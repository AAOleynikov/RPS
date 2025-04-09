/*! @file string_oscillation.сpp
Файл исходного кода основного класса моделирования колебаний струны.
@author Козов А.В., Шам Д.А., Олейников А.А,
@date 2025.04.09 */

#include "string_oscillation.hpp"
#include "single_thread_string_solver.hpp"
#include "multi_thread_string_solver.hpp"
#include <chrono>

// Основной метод для моделирования.
void StringOscillation::simulate(const StringParameters& p) {
  if (p.threads == 1) {
    _solver = std::make_shared<SingleThreadStringSolver>();
  } else {
    _solver = std::make_shared<MultiThreadStringSolver>();
  }

  std::ofstream output;
  output.open(p.output_file);
  if (!p.no_output && !output.is_open()) {
    throw std::runtime_error("StringOscillation: output file name isn't open");
  }
  StringPlotter plotter(output);

  auto start = std::chrono::high_resolution_clock::now();
  _solver->solve(p, plotter);
  auto end = std::chrono::high_resolution_clock::now();

  output.close();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  if (p.no_output) {
    std::cerr << "Threads: " << p.threads 
              << " Time: " << duration.count() << " ms\n";
  }
}
