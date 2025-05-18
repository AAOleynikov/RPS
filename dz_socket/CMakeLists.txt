cmake_minimum_required(VERSION 3.10)
project(echo-application-example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

## Include paths.
include_directories(
  include
)

## Server executable file.
add_executable(multiprocess_server
  src/server/multiprocess_server.cpp
  src/server/server_main.cpp
)

## Client executable file.
add_executable(client
  src/client/client.cpp
  src/client/client_main.cpp
)
