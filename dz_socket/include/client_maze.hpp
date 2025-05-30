/*! @file client_maze.hpp
Заголовочный файл клиента игры "Лабиринт" на базе сокетов Беркли.
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.18 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 4196 ///< Размер буфера чтения, отправки и приёма сообщений.

/// Пространство имён учебных примеров дисциплины "Разработка программных систем".
namespace ssd {

  /*! Класс клиента с использованием сокетов Беркли (протокол TCP). */
  class Client {
  public:

    /*! Создаёт экземпляр клиента.
    @details Конструктор создаёт сокет клиента и связывает его с предоставленным операционной системой адресом. В
    случае ошибки вызывает исключение.
    @throw std::runtime_error При ошибке создания или связывания сокета. */
    Client();

    /*! Закрывает сокет при удалении экземпляра клиента. */
    ~Client();

    /*! Отправляет сообщение серверу и ждёт ответа от него.
    @details Метод устанавливает подключение к серверу с адресом @a h на порт @a p, считывает со стандартного ввода
    сообщение, отправляет его через сокет, ждёт ответа. Полученный ответ от сервера выводит на стандартный вывод.
    Повторяет чтение, отправку и приём @a n раз. В случае ошибки вызывает исключение.
    @throw std::runtime_error При ошибке подключения, оправки или приёма сообщения. */
    void request(const std::string& h, const unsigned short p, const unsigned n, const unsigned s, const bool t);


    /// Закрывает соединение и останавливает приложение
   void stop() {
      if (_socket >= 0) {
        shutdown(_socket, SHUT_RDWR);
        close(_socket);
        _socket = -1;
      }
   }
  protected:

    int _socket; ///< Сокет для подключения к серверу.

  };

} // ssd namespace


#endif // CLIENT_HPP
