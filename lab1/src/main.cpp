#include <iostream>
#include "parameter_parser.hpp"
#include "string_oscillation.hpp"


/// Основная программа.
int main(int argc, char** argv) {
  try {
    ParameterParser parser(argc, argv);
    StringParameters parameters = parser.getParameters();
    StringOscillation s;
    s.simulate(parameters);
  } catch (...) {
    return 1;
  }
  return 0;
}
