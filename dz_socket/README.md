# Реализация сетевого приложения посредством сокетов

Для защиты заданий по сетевому программированию необходимо продемонстрировать работу приложения через локальную сеть, т.е. запустить клиент и сервер на разных компьютерах. Для определения IP-адреса можно использовать утилиты ifconfig или ip addr, для проверки связи — ping HOST.
Одноранговая реализация приложения (см. варианты) предполагает, что такое приложение может быть запущено как в режиме сервера, так и в режиме клиента в зависимости от флага запуска, без перекомпиляции.
Языки разработки — C, C++. Необходимо использовать программный интерфейс сокетов Беркли (Unix-сокетов). Программная документация на приложение должна включать описание разработанного сетевого протокола прикладного уровня, обоснование выбранных структур данных, их описание, программный код.

## Задание - 6.1 Лабиринт (Многопроцессная реализация)

Разработать приложение, реализующее игру «Лабиринт» с компьютерным противником. Игра заключается в отыскании выхода из лабиринта человеком-игроком за заданное число ходов. Программа-клиент инициирует начало игры, отправляя программе-серверу имя игрока. Сервер генерирует случайным образом несколько стен в «лабиринте» размером 3 на 3 клетки. Игрок стартует в левой нижней клетке, его задача — дойти до противоположного угла поля за заданное число ходов-перемещений. Игрок может отправлять сообщения вида «вперёд», «направо», «налево», «назад», «сдаюсь». Сервер принимает сообщения игрока, проверяет их и на каждое сообщение присылает ответ («успешно», «там стена, осталось Y ходов», «вы проиграли», «вы выиграли»,). После завершения игры сетевое соединение завершается. В сетевом протоколе прикладного уровня следует предусмотреть режим тестирования производительности без ограничений на число ходов игрока.
Для игрока необходимо реализовать консольный интерфейс, обеспечивающий удобный (минимально необходимый) ввод с визуализацией хода игры: выполненные ходы и ответы компьютера на них, число ходов, сообщение о победе или проигрыше.

Для приложения необходимо разработать сетевой протокол прикладного уровня на основе TCP.
Приложение-сервер должно поддерживать одновременную работу с неограниченным числом игроков (в пределах вычислительных возможностей компьютера). Сервер необходимо реализовать как многопроцессное приложение с сокетов в блокирующем режиме.
Клиент должен работать по разработанному сетевому протоколу.

## Требования
Для сборки и запуска необходима Unix-система и компилятор C++ с поддержкой стандарта c++11.

Необязательно (в скобка - версия):
- CMake (3.10 и выше);
- Doxygen (1.8 и выше);
- graphviz (7 и выше).

## Сборка и запуск
Для сборки при помощи CMake необходимо выполнить следующие команды в корневой папке проекта:
```bash
mkdir build
cd build
cmake ..
make
```
Собранные таким способом приложения сервера и клиента будут находиться в папке `build`.

Сборка документации к программному коду при помощи Doxygen выполняется при помощи команды
`doxygen Doxyfile` в корневой папке проекта.
Собранная документация расположена в папке  `documentation/html`, для её просмотра нужно открыть файл `index.html` в браузере.