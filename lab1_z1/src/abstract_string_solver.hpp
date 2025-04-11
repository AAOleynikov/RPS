/*! @file abstract_string_solver.hpp
Заголовочный файл абстрактного класса реализации метода решения уравнения струны.
@author Козов А.В.
@date 2025.03.15 */

#ifndef ABSTRACT_STRING_SOLVER_HPP
#define ABSTRACT_STRING_SOLVER_HPP

#include <memory>

#include "string_parameters.hpp"
#include "string_plotter.hpp"


/*! Абстрактный класс реализации метода решения уравнения струны.
@details Класс определяет метод solve для нахождения решения уравнения струны за один временной шаг. */
class AbstractStringSolver {
public:

  /*! Выполняет решение уравнения струны.
  @param p Параметры моделирования струны.
  @param o Объект для вывода решения в файл. */
  virtual void solve(const StringParameters& p, StringPlotter& o) = 0;

  /*! Освобождает память. */
  virtual ~AbstractStringSolver() {};

};


#endif // ABSTRACT_STRING_SOLVER_HPP
