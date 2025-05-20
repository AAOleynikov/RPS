/*! @file server_main.cpp
Главный файл многопроцессного сервера игры "Лабиринт" на базе сокетов Беркли.
@author Козов А.В.
@author Олейников А.А.
@date 2025.05.13 */

#include <iostream>
#include <string>
#include <signal.h>
#include <sys/wait.h> 

#include "multiprocess_maze_server.hpp"

/*! Обрабатывает сигнал завершения работы потомков для предотвращения появления "зомби".
@param s Сигнал. */
void handleSignal(int s) {
  if (s == SIGCHLD) {
    wait(nullptr);
  }
}

/*! Главная функция создаёт экземпляр @p ssd::MultiprocessMazeServer и запускает его основной метод.
@details Функция проверяет аргументы запуска для поиска ключей `-h`, `-p`, задаёт обработчик сигнала завершения
процесса-потомка, создаёт экземпляр сервера @p ssd::MultiprocessMazeServer и запускает его работу вызовом
@p ssd::MultiprocessMazeServer::run . Далее следует блок обработки исключений c последующим выходом из программы.
@dot
  digraph main {
    ranksep=0.25;
    node [shape=box,fontsize="10",fixedsize=true,width=2,height=0.3] # для всех по-умолчанию
    edge [arrowsize=0.5] # для всех по-умолчанию
    Beg [label="Начало",shape=ellipse]
    End [label="Конец",shape=ellipse]
    A [label="Проверка аргументов"]
    B [label="Задание обработчика сигнала"]
    C [label="Создание сервера"]
    D [label="Запуск работы сервера"]
    E [label="Обработка исключений"]
    Beg->A->B->C->D->End
    C->E
    D->E->End
  }
@enddot */
int main(int argc, char** argv) {
  // Обработка аргументов.
  int opt;
  std::string host = "localhost";
  short unsigned port = 1024u;
  while ((opt = getopt(argc, argv, "h:p:")) != -1) {
    switch (opt) {
    case 'h':
      host = optarg;
      break;
    case 'p':
      port = static_cast<unsigned short>(atoi(optarg));
      break;
    default:
      break;
    }
  }
  // Установка обработчика сигнала о завершении работы потомков.
  signal(SIGCHLD, handleSignal);
  // Создание сервера и запуск его работы.
  try {
      ssd::MultiprocessMazeServer server(host, port);
      server.run();
  } catch(const std::runtime_error& e) {
    std::cerr << "EchoServer application error: " << e.what() << std::endl;
    return 1;
  } catch(...) {
    std::cerr << "EchoServer application error: unexpected error" << std::endl;
    return 2;
  }
  // Корректное завершение работы.
  std::cout << "EchoServer application finished" << std::endl;
  return 0;
}
