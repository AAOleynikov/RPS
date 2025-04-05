/*! @file string_oscillation.сpp
Файл исходного кода основного класса моделирования колебаний струны.
@author Козов А.В., Шам Д.А.
@date 2025.03.09 */

#include "string_oscillation.hpp"
#include "single_thread_string_solver.hpp"
//#include "multi_thread_string_solver.hpp"


// Основной метод для моделирования.
void StringOscillation::simulate(const StringParameters& p) {
  _solver = std::make_shared<SingleThreadStringSolver>();

  std::ofstream output;
  output.open(p.output_file);
  if (!p.no_output && !output.is_open()) {
    throw std::runtime_error("StringOscillation: output file name isn't open");
  }
  StringPlotter plotter(output);
  _solver->solve(p, plotter);
  output.close();
}
