/*! @file multi_thread_string_solver.hpp
Заголовочный файл класса многопоточного решателя для моделирования струны.
*/

#ifndef MULTI_THREAD_STRING_SOLVER_HPP
#define MULTI_THREAD_STRING_SOLVER_HPP

#include "abstract_string_solver.hpp"
#include "string_parameters.hpp"
#include <iostream>
#include <thread>
#include <barrier>
using namespace std;

/*! Класс многопоточного решателя для моделирования струны.
@details Класс реализует метод конечных разностей с явной вычислительной схемой для решения уравнения струны в нескольких
потоках выполнения. */
class MultiThreadStringSolver : public AbstractStringSolver {
public:

  /*! Выполняет решение методом конечных разностей с явной вычислительной схемой в нескольких потоках выполнения.
  @warning Предполагается, что значения параметров моделирования струны корректные.
  @param p Параметры моделирования струны.
  @param o Объект для вывода решения в файл. */

  
  void solve(const StringParameters& p, StringPlotter& o) override {
      const double time_step = 0.1 * p.x_step / phase_velocity;
      const unsigned time_step_number = p.duration / time_step;
      unsigned current_time_step = 0u;
      const unsigned solution_size = string_length / p.x_step + 1u;

      auto current = std::make_shared<StringSolution>(solution_size, string_length);
      for (unsigned i = 0u; i < current->size(); i++) {
          current->getValue(i) = p.conditions.begin(i * p.x_step); // начальные условия
      }
      auto last = std::make_shared<StringSolution>(*current);
      auto previous = std::make_shared<StringSolution>(*current);
  
      const unsigned block_size = (solution_size - 2) / p.threads;

      // барьер синхронизации
      std::barrier sync_point(p.threads, [&]{
          if (p.conditions.left.type == BoundaryType::first)
              current->getValue(0) = p.conditions.left.value;
          if (p.conditions.right.type == BoundaryType::first)
              current->getValue(solution_size - 1) = p.conditions.right.value;
          unsigned plot_step = static_cast<unsigned>(0.05 / time_step);
          if (!p.no_output && (current_time_step % plot_step == 0)) {
              o.plot(*current);
          }
          last.swap(current);     
          previous.swap(current);
          current_time_step++;
      });


      // Рабочая функция для потока.
      auto worker = [&](unsigned i_start, unsigned i_end) {          
          StepParam local;
          local.phase_velocity = phase_velocity;
          local.spatial_step = p.x_step;
          local.time_step = time_step;

          while (current_time_step < time_step_number) {
            local.last = last;
            local.previous = previous;
            for (unsigned i = i_start; i < i_end; ++i) {
                local.force = p.conditions.force(i * p.x_step, time_step * current_time_step);
                current->getValue(i) = MKR2(local, i);
            }
            sync_point.arrive_and_wait();
          }
      };
  
      std::vector<std::thread> threads;
      threads.reserve(p.threads);
      for (unsigned t = 0; t < p.threads; ++t) {
          unsigned start = 1 + t * block_size;
          unsigned end = (t == p.threads - 1) ? (solution_size - 1) : (start + block_size);
          threads.emplace_back(worker, start, end);
      }
      for (auto& th : threads)
          th.join();
  }

  protected:
  	// служебные методы.
    /// Вспомогательная структура с параметрами для вычисления одного элемента.
    struct StepParam {
      std::shared_ptr<StringSolution> last;     ///< Указатель на последний слой.
      std::shared_ptr<StringSolution> previous; ///< Указатель на предпоследний слой.
      double time_step;                         ///< Временной шаг.
      double force;
      double spatial_step;                      ///< Пространственный шаг.
      double phase_velocity;                    ///< Фазовая скорость.
    };

    double MKR2(const StepParam& p, unsigned n) {
      const double a_multiplier = p.last->getValue(n + 1) - 2. * p.last->getValue(n) + p.last->getValue(n - 1);
      const double delta_x_square = p.spatial_step * p.spatial_step;
      const double time_multiplier = p.phase_velocity * p.phase_velocity * a_multiplier / delta_x_square;
      const double delta_t_square = p.time_step * p.time_step;
      const double last_term = (time_multiplier + p.force) * delta_t_square;
      const double result = 2. * p.last->getValue(n) - p.previous->getValue(n) + last_term;
      return result;
    }
};

#endif // MULTI_THREAD_STRING_SOLVER_HPP