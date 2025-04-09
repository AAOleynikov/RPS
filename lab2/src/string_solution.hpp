/*! @file string_solution.hpp
Заголовочный файл класса решения уравнения струны для одного временного шага.
@author Козов А.В.
@date 2025.03.15 */

#ifndef STRING_SOLUTION_HPP
#define STRING_SOLUTION_HPP

#include <vector>
#include <stdexcept>


/*! Класс решения уравнения струны для одного временного шага.
@details Класс содержит массив значений фазовой переменной (отклонения точек струны) в узлах пространственной сетки,
длину струны в метрах и обеспечивает доступ к хранимым значениям по индексу узла. */
class StringSolution {
public:

  /*! Создаёт объект решения с заданным числом элементов.
  @throw std::invalid_argument При нулевой или отрицательной длине струны.
  @param s Число элемента массива значений фазовой переменной (число узлов пространственной сетки).
  @param l Длина струны в метрах. */
  explicit StringSolution(unsigned s, double l = 1.) : _length(l), _data() {
    if (_length <= 0.) {
      throw std::invalid_argument("StringSolution: length must be positive");
    }
    _data = std::vector<double>(s);
  };

  /*! Конструктор копирования по умолчанию. */
  StringSolution(const StringSolution&) = default;

  /*! Перемещающий конструктор копирования по умолчанию. */
  StringSolution(StringSolution&&) = default;

  /*! Возвращает размер массива значений фазовой переменной.
  @return Число элементов массива. */
  unsigned size() const {
      return _data.size();
  }

  /*! Возвращает длину струны.
  @return Длина струны, м. */
  double length() const {
    return _length;
  }

  /*! Обеспечивает доступ к элементу массива значений фазовой переменной по индексу.
  @param i Индекс элемента массива.
  @return Ссылка на значение фазовой переменной в узле i.
  @throw std::logic_error При выходе за границы массива. */
  double& getValue(unsigned i) {
    if (i >= _data.size()) {
      throw std::logic_error("StringSolution: invalid index");
    }
    return _data[i];
  }

  /*! Возвращает копию элемента массива значений фазовой переменной по индексу.
  @param i Индекс элемента массива.
  @return Значение фазовой переменной в узле i.
  @throw std::logic_error При выходе за границы массива. */
  double getValue(unsigned i) const {
    if (i >= _data.size()) {
      throw std::logic_error("StringSolution: invalid index");
    }
    return _data[i];
  }

protected:

  double _length;            ///< Длина струны, м.
  std::vector<double> _data; ///< Вектор значений фазовой переменной в узлах сетки

};

#endif // STRING_SOLUTION_HPP
