/*! @file string_oscillation.hpp
Заголовочный файл основного класса приложения для моделирования колебаний струны.
@author Козов А.В., Шам Д.А.
@date 2025.03.09 */

#ifndef STRING_OSCILLATION_HPP
#define STRING_OSCILLATION_HPP

#include <memory>
#include <fstream>


#include "abstract_string_solver.hpp"
#include "string_plotter.hpp"
#include "string_parameters.hpp"


/*! Основной класс приложения для моделирования колебаний струны.
@details @todo */
class StringOscillation {
public:

  /*! Выполняет моделирование с заданными параметрами.
  @warning Предполагается, что значения параметров моделирования струны корректные.
  @param p Параметры моделирования. */
  void simulate(const StringParameters& p);

protected:

  std::shared_ptr<AbstractStringSolver> _solver; ///< Умный указатель на реализацию метода решения.

};


#endif // STRING_OSCILLATION_HPP
