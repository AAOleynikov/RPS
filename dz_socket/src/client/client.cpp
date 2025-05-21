/*! @file client.cpp
Исходный файл клиента игры "Лабиринт" на базе сокетов Беркли.
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.13 */

#include "client_maze.hpp"
#include <cstring>
#include <limits>

//using namespace std; // Вызывает неопределённость при работе с системными вызовами `bind(2)`, `open(2)`!
using namespace ssd;

// Конструктор.
Client::Client() {
  // Создание сокета.
  _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket < 0) {
    throw std::runtime_error("[Client::Client] socket(2) call error");
  }
  // Подготовка сетевого адреса для связывания.
  sockaddr_in address;
  memset(reinterpret_cast<char*>(&address), '\0', sizeof(address));
  // Связывание сокета с сетевым адресом, предоставленным операционной системой.
  if (bind(_socket, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0) {
    throw std::runtime_error("[Client::Client] bind(2) call error");
  }
}

// Деструктор.
Client::~Client() {
  // Закрытие сокета.
  shutdown(_socket, 0);
  close(_socket);
}

void Client::request(const std::string& h, const unsigned short p, const unsigned n, const unsigned s, const bool t) {
  // Подготовка сетевого адреса для подключения.
  sockaddr_in server_address;
  memset(reinterpret_cast<char*>(&server_address), '\0', sizeof(server_address));
  server_address.sin_family = AF_INET;
  hostent* host_name;
  host_name = gethostbyname(h.c_str()); // Преобразование названия хоста в IP-адрес.
  if (host_name == nullptr) {
    throw std::runtime_error("[Client::request] gethostbyname(3) call error");
  }
  server_address.sin_port = htons(p);
  memcpy(&server_address.sin_addr.s_addr, host_name->h_addr, host_name->h_length);

  // Запрос на подключение (установку соединения) к серверу.
  if (connect(_socket, reinterpret_cast<const sockaddr*>(&server_address), sizeof(server_address)) != 0) {
    throw std::runtime_error("[Client::request] connect(2) call error");
  }

  // Отправка серверу длину стороны лабиринта s и максимального числа шагов до поражения
  uint32_t s_s = htonl(static_cast<uint32_t>(s));
  int data_size = write(_socket, &s_s, sizeof(s_s));
  if (data_size < 0) {
    throw std::runtime_error("[Client::request] send(2) call error");
  }
  uint32_t s_n = htonl(static_cast<uint32_t>(n));
  data_size = write(_socket, &s_n, sizeof(s_n));
  if (data_size < 0) {
    throw std::runtime_error("[Client::request] send(2) call error");
  }

  // Запрос имени пользователя
  char* buffer = new char[BUFFER_SIZE];

  if (!t) {
    std::cout << "Введите своё имя перед началом игры\n> ";
    if(!std::cin.get(buffer, BUFFER_SIZE)) {
      std::cout << "[Client::request] Error while reading std::cin" << std::endl;
      return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  
  }
  else {
    const char* test_name_msg = "test_user";
    memcpy(buffer, test_name_msg, std::strlen(test_name_msg));
  }
  buffer[std::strlen(buffer)] = '\0';

  data_size = send(_socket, buffer, std::strlen(buffer)+1, 0);
  if (data_size < 0) {
    delete[] buffer;
    throw std::runtime_error("[Client::request] send(2) call error");
  }

  data_size = read(_socket, buffer, BUFFER_SIZE);
  if (data_size < 0) {
    delete[] buffer;
    throw std::runtime_error("[Client::request] read(2) call error");
  }

  std::cout << buffer << std::endl;

  uint32_t status = 52;
  // Сам процесс игры
  while(true) {
    if (!t) {
      std::cout << "> ";
      if(!std::cin.get(buffer, BUFFER_SIZE)) break;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    else {
      const char* test_msg = "tralalelotralala";
      memcpy(buffer, test_msg, std::strlen(test_msg));
    }

    buffer[std::strlen(buffer)] = '\0';
    data_size = send(_socket, buffer, std::strlen(buffer)+1, 0);
    if (data_size < 0) {
      delete[] buffer;
      throw std::runtime_error("[Client::request] send(2) call error");
    }

    // Чтение кода ((«успешно», «там стена, осталось Y ходов», «вы проиграли», «вы выиграли»,))
    uint32_t old_status = status;
    data_size = read(_socket, &status, sizeof(status));
    if (data_size < 0) {
      delete[] buffer;
      throw std::runtime_error("[Client::request] read(2) call error");
    }
    else if (data_size == 0) {
      if (old_status != 0 && old_status != 2 && old_status != 5) {
        std::cout << "[Client::request] No connection" << std::endl;
      }
      break;
    }
    status = ntohl(status);

    data_size = read(_socket, buffer, BUFFER_SIZE);
    if (data_size < 0) {
      delete[] buffer;
      throw std::runtime_error("[Client::request] read(2) call error");
    }
    else if (data_size == 0) {
      std::cout << "[Client::request] No connection" << std::endl;
      break;
    }   
    
    std::cout << buffer << std::endl;
    if (status == 0 || status == 2 || status == 5) break;
  }
  delete[] buffer;
}
