/*! @file multiprocess_echo_server.cpp
Исходный файл многопроцессного сервера игры "Лабиринт" на базе сокетов Беркли.
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.13 */

#include "multiprocess_maze_server.hpp"
#include "maze.hpp"
#include <cstring>

//using namespace std; // Вызывает неопределённость при работе с системными вызовами `bind(2)`, `open(2)`!
using namespace ssd;

// Конструктор.
MultiprocessMazeServer::MultiprocessMazeServer(const std::string& h, const unsigned short p) {
  // Создание сокета.
  _server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_server_socket < 0) {
    throw std::runtime_error("[MultiprocessMazeServer::MultiprocessMazeServer] socket(2) call error");
  }
  // Подготовка сетевого адреса для связывания.
  sockaddr_in server_address;
  memset(reinterpret_cast<char*>(&server_address), '\0', sizeof(server_address));
  server_address.sin_family = AF_INET;
  hostent* host_name;
  host_name = gethostbyname(h.c_str()); // Преобразование названия хоста в IP-адрес.
  if (host_name == nullptr) {
    throw std::runtime_error("[MultiprocessMazeServer::MultiprocessMazeServer] gethostbyname(3) call error");
  }
  server_address.sin_port = htons(p);
  memcpy(&server_address.sin_addr.s_addr, host_name->h_addr, host_name->h_length);
  // Связывание сокета с заданным сетевым адресом.
  if (bind(_server_socket, reinterpret_cast<const sockaddr*>(&server_address), sizeof(server_address)) != 0) {
    throw std::runtime_error("[MultiprocessMazeServer::MultiprocessMazeServer] bind(2) call error");
  }
  std::cout << "[MultiprocessMazeServer::MultiprocessMazeServer] Ready on " << h << ":" << p << std::endl;
}

// Деструктор.
MultiprocessMazeServer::~MultiprocessMazeServer() {
  // ?
}

// Основной метод.
void MultiprocessMazeServer::run() {
  if (listen(_server_socket, CONNECTION_QUEUE_SIZE) != 0) {
    throw std::runtime_error("[MultiprocessMazeServer::run] listen(2) call error");
  }
  while (1) {
    sockaddr client_address;
    socklen_t address_size = sizeof(client_address);
    // Ожидание запроса подключения клиента для обработки.
    int client_socket = accept(_server_socket, &client_address, &address_size);
    if (client_socket < 0) {
      throw std::runtime_error("[MultiprocessMazeServer::run] accept(2) call error");
    }
    char* client_host = inet_ntoa(reinterpret_cast<sockaddr_in*>(&client_address)->sin_addr);
    unsigned short client_port = ntohs(reinterpret_cast<sockaddr_in*>(&client_address)->sin_port);
    std::cout << "[MultiprocessMazeServer::run] New connection from " << client_host
              << ":" << client_port << std::endl;

    int child_pid = fork();
    if (child_pid < 0) {
      throw std::runtime_error("[MultiprocessMazeServer::run] fork(2) call error");
    } else if (child_pid == 0) {
      // Процесс-потомок работает с клиентом через новый сокет, серверный сокет ему не нужен.
      close(_server_socket);
      handleConnection(client_socket);
      shutdown(client_socket, SHUT_WR);
      close(client_socket);
      break;
    }
    // Процесс-родитель продолжает принимать подключения, клиентский сокет ему не нужен.
    close(client_socket);
  }
}

// Обработка подключения клиента.
void MultiprocessMazeServer::handleConnection(const int s) {
  // Приём длины стороны лабиринта s
  uint32_t maze_size;
  int length = read(s, &maze_size, sizeof(maze_size));
  maze_size = ntohl(maze_size);
  if (length < 0) {
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] recv(2) call error");
  } else if (length == 0) {
      std::cout << "[MultiprocessMazeServer::handleConnection] No connection" << std::endl;
      return;
  }

  uint32_t n;
  length = read(s, &n, sizeof(n));
  n = ntohl(n);
  if (length < 0) {
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] recv(2) call error");
  } else if (length == 0) {
      std::cout << "[MultiprocessMazeServer::handleConnection] No connection" << std::endl;
      return;
  }

  // Создаём объект лабиринт размера maze_size
  std::cout << maze_size << std::endl;
  std::cout << n << std::endl;
  Maze maze(maze_size);
  uint32_t game_status; // 0 - Выход, 1 - Неизвестная комманда, 2 - Победа, 3 - Ход совершён, 4 - Встречена стена, 5 - Проигрыш
  // Получаем ASCII представление текущего лабиринта
  std::string maze_ascii = maze.toAscii();

  // Создание буфера для приёма и отправки сообщений.
  char* buffer = new char[BUFFER_SIZE];
  // Количество записанных байт в буффер
  int buff_shift = 0;
  std::sprintf(buffer, "Добро пожаловать в игру Лабиринт! \n\
Для управления вводите комманды: вперёд, назад, направо, налево\n\
Для досрочного выхода ведите: выход\n\
Осталось ходов: %d\n%s", n, maze_ascii.c_str());  

  length = send(s, buffer, std::strlen(buffer), 0);
  if (length < 0) {
    delete[] buffer;
    throw std::runtime_error("[MultiprocessMazeServer::handleConnection] send(2) call error");
  }

  for (int i = n; i > 0; i--) {
    buff_shift = 0;
    int length = read(s, buffer, BUFFER_SIZE);
    if (length < 0) {
      delete[] buffer;
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] recv(2) call error");
    } else if (length == 0) {
      std::cout << "[MultiprocessMazeServer::handleConnection] No connection" << std::endl;
      break;
    }
    std::cout << buffer << std::endl;

    // Смещение по клеткам лабиринта
    int dx=0, dy=0;
    // Пришедшая от пользователя комманда
    Command cmd = parse_command(buffer);
    switch (cmd) {
      case Command::FORWARD:  
        dy = -1; 
        break;
      case Command::BACKWARD: 
        dy = 1;  
        break;
      case Command::RIGHT:    
        dx = 1;  
        break;
      case Command::LEFT:     
        dx = -1; 
        break;
      case Command::EXIT: {
          game_status = htonl(0);
          const char* msg_exit = "Вы сдались. Игра окончена.\n";
          memcpy(buffer+buff_shift, msg_exit, std::strlen(msg_exit));
          buff_shift += std::strlen(msg_exit);
          break;
      }

      default: {
          game_status = htonl(1);
          const char* msg_uncnown = "Неизвестная комманда, попробуйте снова.\n";
          memcpy(buffer+buff_shift, msg_uncnown, std::strlen(msg_uncnown));
          buff_shift += std::strlen(msg_uncnown);
          i++; // не забираем у пользователя попытку
          break;
      }
    }

    if (cmd != Command::UNKNOWN && cmd != Command::EXIT) {
      // Пользователь хочет совершить ход, проверяем есть ли у него возможность сдвинуться в ту сторону
      // Если true, то пользователь получил новую позицию, иначе - нет
      bool ability = maze.movePlayer(dx, dy);
      // Если новая позиция выйгрышная
      if (maze.playerWin()){
        game_status = htonl(2);
        const char* msg_win = "Вы выйграли! Поздравляем!\n";
        memcpy(buffer+buff_shift, msg_win, std::strlen(msg_win));
        buff_shift += std::strlen(msg_win);
      }
      // Если новая позиция не выйгрышная
      else {
        // Если ход был совершён
        if (ability) {
          game_status = htonl(3);
          const char* move_msg = "Отличный ход!\n";
          memcpy(buffer+buff_shift, move_msg, std::strlen(move_msg));
          buff_shift += std::strlen(move_msg);
        } 
        // Если позиция не была измена
        else {
          game_status = htonl(4);
          const char* no_move_msg = "Упс... Стена:)\n";
          memcpy(buffer+buff_shift, no_move_msg, std::strlen(no_move_msg));
          buff_shift += std::strlen(no_move_msg);
        }
        // Сколько ходов осталось + текущий лабиринт в ASCII
        if (i > 1) {
          maze_ascii = maze.toAscii();
          std::string moves_left_msg = "Осталось ходов: " + std::to_string(i-1) + "\n";
          memcpy(buffer+buff_shift, moves_left_msg.c_str(), moves_left_msg.length());
          buff_shift += moves_left_msg.length();
          memcpy(buffer+buff_shift, maze_ascii.c_str(), maze_ascii.length());
          buff_shift += maze_ascii.length();
        }
        else {
          game_status = htonl(5);
          break;
        }
      }
    } // end if Command::EXIT
    
    length = send(s, &game_status, sizeof(game_status), 0);
    if (length < 0) {
      delete[] buffer;
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] send(2) call error");
    }
    else if (length == 0) {
      std::cout << "[Client::request] No connection" << std::endl;
      break;
    }
    
    buffer[buff_shift] = '\n';
    buffer[buff_shift+1] = '\0';
    length = send(s, buffer, buff_shift+2, 0);
    if (length < 0) {
      delete[] buffer;
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] send(2) call error");
    }
    else if (length == 0) {
      std::cout << "[Client::request] No connection" << std::endl;
      break;
    }

    if (cmd == Command::EXIT) {
      break; 
    } 
    if (game_status == 0 || game_status == 2 || game_status == 5) break;
  } // end for n
  
  if (game_status != 0 && game_status != 2) {
    game_status = htonl(5);
    buff_shift = 0;
    const char* defeat_msg = "О нет... Похоже попыток больше нет и вы проиграли.\n@}‑;‑'‑‑‑";
    memcpy(buffer+buff_shift, defeat_msg, std::strlen(defeat_msg));
    buff_shift += std::strlen(defeat_msg);
    buffer[buff_shift] = '\n';
    buffer[buff_shift+1] = '\0';

    length = send(s, &game_status, sizeof(game_status), 0);
    if (length < 0) {
      delete[] buffer;
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] send(2) call error");
    }
    else if (length == 0) {
      std::cout << "[Client::request] No connection" << std::endl;

    }
    else {
    length = send(s, buffer, buff_shift+2, 0);
    if (length < 0) {
      delete[] buffer;
      throw std::runtime_error("[MultiprocessMazeServer::handleConnection] send(2) call error");
    }
    else if (length == 0) {
      std::cout << "[Client::request] No connection" << std::endl;
    }

    std::cout << "Сообщение о конце игры отправлено пользователю!!" << std::endl;
    std::cout << buffer << std::endl;
    }
  }

  delete[] buffer;
}
