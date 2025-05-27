/*! @file mpi_main.cpp
Главный файл приложения моделирования колебаний струны с использованием MPI (лабораторная работа №2 по курсу "РПС").
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.20 */

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>

#include "parameter_parser.hpp"


/*! Вспомогательная структура с данными для расчёта значения узла сетки */
struct NodeParameters {
  double* last;        ///< Указатель на данные временного слоя j.
  double* previous;    ///< Указатель на данные временного слоя j - 1.
  double spatial_step; ///< Пространственный шаг, м.
  double time_step;    ///< Временной шаг, с.
};

/*! Рассчитывает значение в узле @a n по заданным параметрам.
@param p Параметры для расчёта.
@param f Сила, приложенная к узлу.
@param n Номер рассчитываемого узла.
@return Рассчитанное значение. */
double calculateNode(const NodeParameters& p, double f, unsigned long n) {
  const double a_multiplier = p.last[n + 1] - 2. * p.last[n] + p.last[n - 1];
  const double delta_x_square = p.spatial_step * p.spatial_step;
  const double time_multiplier = phase_velocity * phase_velocity * a_multiplier / delta_x_square;
  const double delta_t_square = p.time_step * p.time_step;
  const double last_term = (time_multiplier + f) * delta_t_square;
  const double result = 2. * p.last[n] - p.previous[n] + last_term;
  return result;
}

/*! Главная функция приложения. */
int main(int argc, char** argv) {
  auto result = MPI_Init(&argc, &argv);
  if (result != MPI_SUCCESS) {
    std::cerr << " Can't initialize MPI\n";
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_OTHER);
  }
  int my_rank, total;
  MPI_Comm_size(MPI_COMM_WORLD, &total);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  //std::cout << "  > Total processes " << total << ", my rank " << my_rank << std::endl;
  ParameterParser parser(argc, argv);
  StringParameters parameters = parser.getParameters();
  const unsigned long solution_size = round(string_length / parameters.spatial_step) + 1ul;
  std::ofstream output;
  auto begin_calculation_time = std::chrono::system_clock::now();

  double* init_data;
  const unsigned parameter_buffer_size = 5;
  double parameters_buffer[parameter_buffer_size];
  if (my_rank == 0) { // "основной" процесс
    parameters_buffer[0] = parameters.spatial_step;
    parameters_buffer[1] = solution_size;
    parameters_buffer[2] = 0.1 * parameters.spatial_step / phase_velocity;
    parameters_buffer[3] = round(parameters.duration / parameters_buffer[2]);
    parameters_buffer[4] = (parameters.no_output ? 1. : 0.);
    // Начальные условия.
    init_data = new double[solution_size];
    for (unsigned long i = 0lu; i < solution_size; i++) {
      const double x = i * parameters.spatial_step;
      init_data[i] = parameters.conditions.begin(x);
    }
    if (!parameters.no_output) {
      output.open(parameters.output_file);
    }
    // Данные готовы к рассылке.
  }
  // Передача всем процессам информации о задаче.
  MPI_Bcast(parameters_buffer, parameter_buffer_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  const double local_spatial_step = parameters_buffer[0];
  const unsigned long local_spatial_step_number = parameters_buffer[1];
  const double local_time_step = parameters_buffer[2];
  const unsigned long local_time_step_number = parameters_buffer[3];
  const bool local_output_flag = parameters_buffer[4];

  // Распределение данных между процессами.
  unsigned long local_size = local_spatial_step_number / total;
  if (my_rank == total - 1) {
    local_size += local_spatial_step_number % total;
  }
  double* local_current = new double[local_size + 2ul];
  double* local_last = new double[local_size + 2ul];
  double* local_previous = new double[local_size + 2ul];
  int elements_numer_per_process[total];
  int indices_per_process[total];
  for (int i = 0u; i < total; i++) {
    elements_numer_per_process[i] = local_spatial_step_number / total;
    indices_per_process[i] = i * (local_spatial_step_number / total);
  }
  elements_numer_per_process[total - 1] += local_spatial_step_number % total;
  MPI_Scatterv(init_data, elements_numer_per_process, indices_per_process, MPI_DOUBLE,
               local_last + 1u, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // Теперь все процессы обладают начальными значениями узлов.
  for (unsigned long i = 0ul; i < local_size + 2; i++) {
    local_previous[i] = local_last[i];
  }
  // Номера процессов, обрабатывающих данные "слева" и "справа" от текущего.
  int left_rank = (my_rank > 0) ? my_rank - 1 : MPI_PROC_NULL;
  int right_rank = (my_rank < total - 1) ? my_rank + 1 : MPI_PROC_NULL;
  unsigned long current_time_step = 1ul;
  // Цикл по времени моделирования.
  while (current_time_step <= local_time_step_number) {
    // Обмен данными на границах.
    MPI_Sendrecv(local_last + 1, 1, MPI_DOUBLE, left_rank, 0,
                 local_last + local_size + 1, 1, MPI_DOUBLE, right_rank, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv(local_last + local_size, 1, MPI_DOUBLE, right_rank, 1,
                 local_last, 1, MPI_DOUBLE, left_rank, 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    NodeParameters calculate_parameters;
    calculate_parameters.last = local_last;
    calculate_parameters.previous = local_previous;
    calculate_parameters.time_step = local_time_step;
    calculate_parameters.spatial_step = local_spatial_step;
    // Цикл по узлам сетки.
    for (unsigned long i = 1ul; i < local_size + 1; i++) {
      if (my_rank == 0 && i == 1ul) {
        if (parameters.conditions.left.type == BoundaryType::first) {
          local_current[i] = parameters.conditions.left.value;
        }
      } else if (my_rank == total - 1 && i == local_size) {
        if (parameters.conditions.right.type == BoundaryType::first) {
          local_current[i] = parameters.conditions.right.value;
        } else {
          local_current[i] = local_current[i-1]; // рассм. гр. усл. ...=0
        }
      } else {
        const double x = (my_rank * local_spatial_step_number / total + i - 1) * local_spatial_step;
        const double force = parameters.conditions.force(x, current_time_step * local_time_step);
        local_current[i] = calculateNode(calculate_parameters, force, i);
      }
    }
    // Для "реального" масштаба времени анимации выводится не более 20 кадров на 1 секунду модельного времени.
    unsigned long plot_step = 0.05 / local_time_step;
    if (!local_output_flag && current_time_step % plot_step == 0) {
      MPI_Gatherv(local_current + 1, local_size, MPI_DOUBLE, init_data,
                  elements_numer_per_process, indices_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      if (my_rank == 0) {
        // Вывод только в корневом процессе, только он обладает данными по всем узлам!
        const double step = string_length / (solution_size - 1);
        for (unsigned long i = 0ul; i < solution_size; i++) {
          output << '\n' << i * step << " " << init_data[i];
        }
        output << "\n\n";
      }
    }

    double* temp_pointer = local_previous;
    local_previous = local_last;
    local_last = local_current;
    local_current = temp_pointer;
    current_time_step++;
  }
  delete[] local_current;
  delete[] local_last;
  delete[] local_previous;
  if (my_rank == 0) {
    delete[] init_data;
  }
  MPI_Finalize();

  if (parameters.no_output) {
    auto duration = std::chrono::system_clock::now() - begin_calculation_time;
    //std::cout << "  > Computation time " << std::chrono::duration<double>(duration).count() << " s" << std::endl;
    std::cout << std::chrono::duration<double>(duration).count() << std::endl;
  }
  return 0;
}
