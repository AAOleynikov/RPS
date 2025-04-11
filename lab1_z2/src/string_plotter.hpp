/*! @file string_plotter.hpp
Заголовочный файл класса формирования файла данных для gnuplot.
@author Козов А.В.
@date 2025.03.15 */

#ifndef STRING_PLOTTER_HPP
#define STRING_PLOTTER_HPP

#include <ostream>

#include "string_solution.hpp"


/*! Класс формирования файла данных для gnuplot.
@details Класс содержит ссылку на поток вывода и метод для формирования блока данных для объекта @p StringSolution с
решением уравнения струны для одного временного шага. */
class StringPlotter {
public:

  /*! Создаёт объект вывода по ссылке на поток вывода.
  @param o Ссылка на поток вывода.  */
  explicit StringPlotter(std::ostream& o) : _output(o) {};

  /*! Выводит значения из @p StringSolution в виде пар "x u" в одном блоке данных gnuplot.
  @param s Выводимый массив значений. */
  void plot(const StringSolution& s) {
    const double step = s.length() / (s.size() - 1); // size - число узлов, интервалов на 1 меньше
    for (unsigned i = 0u; i < s.size(); i++) {
      _output << '\n' << i * step << " " << s.getValue(i);
    }
    _output << "\n\n";
  }

protected:

  std::ostream& _output; ///< Ссылка на поток вывода.

};


#endif // STRING_PLOTTER_HPP
