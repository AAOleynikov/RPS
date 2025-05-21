/*! @file client_main.cpp
Главный файл клиента игры "Лабиринт" на базе сокетов Беркли.
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.13 */

#include <iostream>
#include <csignal>
#include "client_maze.hpp"

static ssd::Client* g_client_ptr = nullptr;
void on_sigint(int) {
  std::cerr << "\nReceived SIGINT, shutting down client...\n";
  if (g_client_ptr) {
    g_client_ptr->stop();
  }
  std::exit(0);
}

/*! Главная функция создаёт экземпляр @p pd::Client и запускает его основной метод @p pd::Client::request.
@details Функция проверяет аргументы запуска для поиска ключей `-h`, `-p`, `-n`, `-s`, `-t`, создаёт экземпляр клиента @p
pd::Client и запускает его работу вызовом @p pd::Client::request . Далее следует блок обработки исключений и выход
из программы. */
int main(int argc, char** argv) {
  // Обработка аргументов.
  int opt;
  std::string host = "localhost";
  short unsigned port = 1024u;
  unsigned number = 10;
  unsigned maze_size = 3;
  bool test_mode = false;
  std::signal(SIGINT, on_sigint);
  while ((opt = getopt(argc, argv, "h:p:n:s:t")) != -1) {
    switch (opt) {
    case 'h': // host сервера
      host = optarg;
      break;
    case 'p': // port сервера
      port = static_cast<unsigned short>(atoi(optarg));
      break;
    case 'n': // количество ходов, которые можно совершить
      number = static_cast<unsigned>(atoi(optarg));
      if (number <= 0) {
        throw std::invalid_argument("StringParameters: Количество ходов при инициализации должно быть больше 0");
      }
      break;
    case 's': // длина стороны поля лабиринта
      maze_size = static_cast<unsigned>(atoi(optarg));
      if (maze_size <= 2) {
        throw std::invalid_argument("StringParameters: Длина стороны лабиринта должна быть больше или равна 3");
      }
      break;
    case 't': // режим для тестирования
      test_mode = true;
      break;
    default:
      break;
    }
  }

  // Создание клиента и запуск его работы.
  try {
    ssd::Client client;
    g_client_ptr = &client;
    client.request(host, port, number, maze_size, test_mode);
  } catch(const std::runtime_error& e) {
    std::cerr << "Client application error: " << e.what() << std::endl;
    return 1;
  } catch(...) {
    std::cerr << "Client application error: unexpected error" << std::endl;
    return 2;
  }
  // Корректное завершение работы.
  std::cout << "Client application finished" << std::endl;
  return 0;
}