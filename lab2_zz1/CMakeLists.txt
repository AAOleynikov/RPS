cmake_minimum_required(VERSION 3.5)
project(mpi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

## Проверка наличия утилиты gnuplot.
find_program(GNUPLOT gnuplot)
if (NOT GNUPLOT)
  message(WARNING "gnuplot not found!")
endif()

## Исполняемый файл проекта.
add_executable(oscillation_mpi_main
  src/mpi_main.cpp
)
target_link_libraries(oscillation_mpi_main ${MPI_CXX_LIBRARIES})
