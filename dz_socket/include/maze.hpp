/*! @file maze.hpp
Заголовочный файл игры "Лабиринт".
@author Олейников А.А.
@date 2025.05.13 */

#ifndef MAZE_HPP
#define MAZE_HPP

#include <cstring>       
#include <vector>       
#include <string>       
#include <random>        
#include <algorithm>     
#include <sstream>      

/// Пространство имён учебных примеров дисциплины "Разработка программных систем".
namespace ssd
{
    /*!
    @class Maze
    @brief Класс, реализующий игровой лабиринт.
    */
    class Maze
    {
    public:
        /*!
        @brief Конструктор лабиринта.
         
        @param n Размер стороны лабиринта.
        */
        Maze(unsigned n) : size(n),
                           h_wall(n + 1, std::vector<bool>(n, true)),
                           v_wall(n, std::vector<bool>(n + 1, true))
        {
            // Инициализация игрока и выхода
            player_x = 0;
            player_y = n - 1;
            exit_x   = n - 1;
            exit_y = 0;
            generateMaze(player_x, player_y);;
        }

        /*!
        @brief Проверяет, достиг ли игрок выхода.
         
        @return true, если игрок достиг выхода.
        @return false, если игрок ещё не достиг выхода.
        */
        bool playerWin() {
            if (player_x == exit_x && player_y == exit_y) {
                return true;
            }
            return false;
        } 

        /*!
        @brief Возвращает строку ASCII-графики текущего состояния лабиринта.
         
        @return std::string ASCII-графика лабиринта.
        */
        std::string toAscii()
        {
            std::ostringstream out;
            // Верхняя граница
            out << "+";
            for (int x = 0; x < size; x++)
            {
                out << (h_wall[0][x] ? "--+" : "  +");
            }
            out << "\n";
            // Для каждой строки
            for (int y = 0; y < size; y++)
            {
                // Строка с вертикальными стенами и ячейками
                out << "|";
                for (int x = 0; x < size; x++)
                {
                    // Содержимое ячейки
                    char c = ' ';
                    if (x == player_x && y == player_y)
                        c = 'P';
                    else if (x == exit_x && y == exit_y)
                        c = 'E';
                    out << " " << c;
                    // Правая стенка
                    out << (v_wall[y][x + 1] ? "|" : " ");
                }
                out << "\n";
                // Горизонтальная граница ниже
                out << "+";
                for (int x = 0; x < size; x++)
                {
                    out << (h_wall[y + 1][x] ? "--+" : "  +");
                }
                out << "\n";
            }
            return out.str();
        }

        /*!
        @brief Пытается переместить игрока.
         
        @param dx Смещение по оси X.
        @param dy Смещение по оси Y.
        @return true, если перемещение возможно и выполнено.
        @return false, если перемещение невозможно (есть стена или выход за границы).
        */
        bool movePlayer(int dx, int dy)
        {
            // Попытка движения: dx,dy = {0,-1}(вверх), {0,1}(вниз), {1,0}(вправо), {-1,0}(влево)
            int nx = player_x + dx, ny = player_y + dy;
            if (nx < 0 || ny < 0 || nx >= size || ny >= size)
                return false;
            // Проверяем стену на пути
            if (dx == 1)
            { // вправо
                if (v_wall[player_y][player_x + 1])
                    return false;
            }
            else if (dx == -1)
            { // влево
                if (v_wall[player_y][player_x])
                    return false;
            }
            else if (dy == -1)
            { // вверх
                if (h_wall[player_y][player_x])
                    return false;
            }
            else if (dy == 1)
            { // вниз
                if (h_wall[player_y + 1][player_x])
                    return false;
            }
            player_x = nx;
            player_y = ny;
            return true;
        }

    private:
        unsigned size;                         ///< Длина стороны лабиринта
        std::vector<std::vector<bool>> h_wall; ///< Горизонтальные стены (размером (size+1) x size)
        std::vector<std::vector<bool>> v_wall; ///< Вертикальные стены (размером size x (size+1))
        int player_x, player_y;                ///< Координаты игрока
        int exit_x, exit_y;                    ///< Координаты выхода из лабиринта

        /*!
        @brief Генерирует лабиринт с помощью алгоритма DFS, начиная с заданной ячейки.
    
        @param sx X-координата стартовой ячейки для DFS
        @param sy Y-координата стартовой ячейки для DFS
        */
        void generateMaze(int sx, int sy)
        {
            std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
            carve(sx, sy, visited);
        }

        /*!
        @brief Рекурсивная функция для построения проходов.
         
        @param cx Текущая координата X.
        @param cy Текущая координата Y.
        @param vis Матрица посещённых ячеек.
        */
        void carve(int cx, int cy, std::vector<std::vector<bool>> &vis)
        {
            vis[cy][cx] = true;
            // случайный порядок направлений
            std::vector<int> dirs = {0, 1, 2, 3}; // 0:up, 1:right, 2:down, 3:left
            std::shuffle(dirs.begin(), dirs.end(), std::mt19937{std::random_device{}()});
            for (int d : dirs)
            {
                int nx = cx, ny = cy;
                if (d == 0)
                    ny = cy - 1;
                if (d == 1)
                    nx = cx + 1;
                if (d == 2)
                    ny = cy + 1;
                if (d == 3)
                    nx = cx - 1;
                if (nx < 0 || ny < 0 || nx >= size || ny >= size)
                    continue;
                if (vis[ny][nx])
                    continue;
                // Убрать стену между (cx,cy) и (nx,ny)
                if (d == 0)
                {
                    h_wall[cy][cx] = false;
                }
                else if (d == 2)
                {
                    h_wall[cy + 1][cx] = false;
                }
                else if (d == 3)
                {
                    v_wall[cy][cx] = false;
                }
                else if (d == 1)
                {
                    v_wall[cy][cx + 1] = false;
                }
                carve(nx, ny, vis);
            }
        }
    };

} // ssd namespace

#endif // MAZE_HPP
