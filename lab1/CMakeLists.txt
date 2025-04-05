cmake_minimum_required(VERSION 3.5)
project(multithread)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

## Поиск gnuplot'а
find_program(GNUPLOT gnuplot)
if (NOT GNUPLOT)
  message(WARNING "gnuplot not found!")
endif()

add_executable(oscillation_main
  src/main.cpp
  src/string_oscillation.cpp
)
