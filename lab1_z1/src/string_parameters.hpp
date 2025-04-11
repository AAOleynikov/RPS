/*! @file string_parameters.hpp
Заголовочный файл структуры с параметрами моделирования струны.
@author Козов А.В.
@date 2025.03.09 */

#ifndef STRING_PARAMETERS_HPP
#define STRING_PARAMETERS_HPP

#include <functional>
#include <string>


/// Глобальная константа с длиной струны, м.
const double string_length = 1.;

/// Глобальная константа с фазовой скоростью, м/с.
const double phase_velocity = 1.;

/// Типы граничных условий.
enum class BoundaryType {
  first, ///< Первого рода.
  second ///< Второго рода.
};

/// Структура описания граничного условия.
struct Boundary {
  BoundaryType type; ///< Тип.
  double value;      ///< Значение.
};

/*! Структура с начальным и граничными условиями моделирования. */
struct Conditions {
  std::function<double(double, double)> force; ///< Внешняя сила p(x, t).
  std::function<double(double)> begin;         ///< Начальные условия.
  Boundary left;                               ///< Граничное условие для левого конца.
  Boundary right;                              ///< Граничное условие для правого конца.
};

/*! Служебная структура с параметрами моделирования струны. */
struct StringParameters {
  double duration;         ///< Продолжительность моделирования.
  double x_step;           ///< Пространственный шаг сетки.
  unsigned threads;        ///< Число потоков выполнения.
  bool no_output;          ///< Флаг служебного режима без файлового вывода.
  std::string output_file; ///< Название файла для вывода результатов моделирования.
  Conditions conditions;   ///< Начальные и граничные условия.
};


#endif // STRING_PARAMETERS_HPP
