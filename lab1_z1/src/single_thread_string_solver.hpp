/*! @file single_thread_string_solver.hpp
Заголовочный файл класса однопоточного решателя для моделирования струны.
@author Козов А.В., Шам Д.А.
@date 2025.03.15 */

#ifndef SINGLE_THREAD_STRING_SOLVER_HPP
#define SINGLE_THREAD_STRING_SOLVER_HPP

#include "abstract_string_solver.hpp"
#include "string_parameters.hpp"
#include <iostream>
using namespace std;


/*! Класс однопоточного решателя для моделирования струны.
@details Класс реализует метод конечных разностей с явной вычислительной схемой для решения уравнения струны в одном
потоке выполнения. */
class SingleThreadStringSolver : public AbstractStringSolver {
public:

  /*! Выполняет решение методом конечных разностей с явной вычислительной схемой в одном потоке выполнения.
  @warning Предполагается, что значения параметров моделирования струны корректные.
  @param p Параметры моделирования струны.
  @param o Объект для вывода решения в файл. */
  void solve(const StringParameters& p, StringPlotter& o) override {
    const double time_step = 0.1 * p.x_step / phase_velocity;
    const unsigned time_step_number = p.duration / time_step;
    unsigned current_time_step = 0u;
    const unsigned solution_size = string_length / p.x_step + 1u; // число узлов сетки
    
    auto current = std::make_shared<StringSolution>(solution_size, string_length);
    
    for (unsigned i = 0u; i < current->size(); i++) {
      const double x = i * p.x_step;
      current->getValue(i) = p.conditions.begin(x); // начальные условия
    }
    
    auto last = std::make_shared<StringSolution>(*current);
    auto previous = std::make_shared<StringSolution>(*current);

    while (current_time_step <= time_step_number) {
      // Расчёт значений по явной вычислительной схеме.
      StepParam parameters;
      parameters.last = last;
      parameters.previous = previous;
      parameters.phase_velocity = phase_velocity;
      parameters.spatial_step = p.x_step;
      parameters.time_step = time_step;
      // Цикл по узлам сетки.
      for (unsigned i = 0u; i < current->size(); i++) {
		if (i == 0u) {
		  if (p.conditions.left.type == BoundaryType::first) {
			 current->getValue(i) = p.conditions.left.value;
		  } else {
			  // TODO
		  }
		} else if (i == current->size() - 1) {
		  if (p.conditions.right.type == BoundaryType::first) {
			 current->getValue(i) = p.conditions.right.value;
		  } else {
			  // TODO
		  }
		} else {
          parameters.force = p.conditions.force(i * p.x_step, time_step * current_time_step);
          current->getValue(i) = MKR2(parameters, i);
	    }
      }
      // Вывод решения.
      unsigned plot_step = 0.05 / time_step; // для "реального масштаба" времени анимации
      if (!p.no_output && current_time_step % plot_step == 0) {
        o.plot(*current);
      }

      // Обмен указателями для повышения быстродействия.
      last.swap(current);     // в last теперь текущий слой, в current теперь слой j
      previous.swap(current); // в previous теперь слой j, в current теперь слой j-1
      
      // Следующий шаг по времени.
      current_time_step++;
    }
  }

  protected:
  
	// служебные методы.

    /// Вспомогательная структура с параметрами для вычисления одного элемента.
    struct StepParam {
      std::shared_ptr<StringSolution> last;     ///< Указатель на последний слой.
      std::shared_ptr<StringSolution> previous; ///< Указатель на предпоследний слой.
      double force;                             ///< Значение силы.
      double time_step;                         ///< Временной шага.
      double spatial_step;                      ///< Пространственный шага.
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


#endif // SINGLE_THREAD_STRING_SOLVER_HPP
