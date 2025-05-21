/*! @file client.cpp
Исходный файл клиента игры "Лабиринт" на базе сокетов Беркли.
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.13 */

#include "client_maze.hpp"
#include <vector>
#include <cstring>
#include <random>
#include <limits>
#include <iostream>
#include <thread>
#include <chrono>

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
  if (_socket >= 0) {
    shutdown(_socket, SHUT_RDWR);
    close(_socket);
   _socket = -1;
  }
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

    // Отправляем параметры s и n
    uint32_t net_s = htonl(static_cast<uint32_t>(s));
    if (write(_socket, &net_s, sizeof(net_s)) < 0)
        throw std::runtime_error("[Client::request] send(2) call error");

    uint32_t net_n = htonl(static_cast<uint32_t>(n));
    if (write(_socket, &net_n, sizeof(net_n)) < 0)
        throw std::runtime_error("[Client::request] send(2) call error");

    std::vector<char> buffer(BUFFER_SIZE);

    // Читаем или генерируем имя
    if (!t) {
        std::cout << "Введите своё имя перед началом игры\n> ";
        if (!std::cin.get(buffer.data(), BUFFER_SIZE))
            return;                              // ничего не сломается, buffer очистится автоматически
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else {
        const char* test_name = "test_user";
        std::strncpy(buffer.data(), test_name, BUFFER_SIZE-1);
    }
    if (!t) buffer[std::strlen(buffer.data())] = '\0';

    // Отправляем имя
    if (send(_socket, buffer.data(), std::strlen(buffer.data())+1, 0) < 0)
        throw std::runtime_error("[Client::request] send(2) call error");

    // Принимаем приветствие
    ssize_t len = read(_socket, buffer.data(), BUFFER_SIZE);
    if (len < 0)
        throw std::runtime_error("[Client::request] read(2) call error");
    if(!t) std::cout.write(buffer.data(), len);
    std::cout << "\n";

    uint32_t status = 0;
    // Игровой цикл
    while (true) {
        // Считываем ход
        if (!t) {
            std::cout << "> ";
            if (!std::cin.get(buffer.data(), BUFFER_SIZE)) break;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            // по логике работы сервера на некорректные комманды он не снижает попытки
            const char* test_cmd = "tesc_cmd";
            std::strncpy(buffer.data(), test_cmd, BUFFER_SIZE-1);
            //for (int i = 0; i < 10000; i++)
            //    for (int j = 0; j < 1000; j++);
        }
        buffer[std::strlen(buffer.data())] = '\0';

        // Отправляем команду
        if (send(_socket, buffer.data(), std::strlen(buffer.data())+1, 0) < 0)
            throw std::runtime_error("[Client::request] send(2) call error");

        // Читаем код состояния игровой сессии
        uint32_t net_status;
        ssize_t r = read(_socket, &net_status, sizeof(net_status));
        if (r < 0)
            throw std::runtime_error("[Client::request] read(2) call error");
        if (r == 0) break;  // соединение закрылось
        status = ntohl(net_status);

        // Читаем ASCII-ответ
        r = read(_socket, buffer.data(), BUFFER_SIZE);
        if (r < 0)
            throw std::runtime_error("[Client::request] read(2) call error");
        if (r == 0) break;  // соединение закрылось

        std::cout.write(buffer.data(), r);
        if (!t) std::cout << "\n";

        if (status == 0 || status == 2 || status == 5) break;
    }
}