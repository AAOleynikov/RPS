/*! @file client_main.cpp
Главный файл клиента эхо-сервера на базе сокетов Беркли.
@author Козов А.В.
@date 2024.01.03 */

#include <iostream>
#include "client.hpp"

/*! Главная функция создаёт экземпляр @p pd::Client и запускает его основной метод @p pd::Client::request.
@details Функция проверяет аргументы запуска для поиска ключей `-h`, `-p`, `-n`, создаёт экземпляр клиента @p
pd::Client и запускает его работу вызовом @p pd::Client::request . Далее следует блок обработки исключений  и выход
из программы. */
int main(int argc, char** argv) {
  // Обработка аргументов.
  int opt;
  std::string host = "localhost";
  short unsigned port = 1024u;
  unsigned number = 10;
  unsigned maze_size = 3;
  bool test_mode = false;
  while ((opt = getopt(argc, argv, "h:p:n:s:t")) != -1) {
    switch (opt) {
    case 'h':
      host = optarg;
      break;
    case 'p':
      port = static_cast<unsigned short>(atoi(optarg));
      break;
    case 'n':
      number = static_cast<unsigned>(atoi(optarg));
      break;
    case 's':
      maze_size = static_cast<unsigned>(atoi(optarg));
      break;
    case 't':
      test_mode = true;
      break;
    default:
      break;
    }
  }

  // Создание клиента и запуск его работы.
  try {
    ssd::Client client;
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