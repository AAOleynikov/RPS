/*! @file maze.hpp
Заголовочный файл игры Лабиринт.
@author Олейников А.А.
@date 2024.05.13 */

#ifndef MAZE_HPP
#define MAZE_HPP

#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <signal.h>
#include <sstream>

/// Пространство имён учебных примеров дисциплины "Разработка программных систем".
namespace ssd
{
    enum class Command {
        FORWARD,  // вперёд
        BACKWARD, // назад
        RIGHT,    // направо
        LEFT,     // налево
        EXIT,     // выход
        UNKNOWN   // неопознанная команда
    };

    struct Keyword {
        const char* word;    // Строка-ключ
        Command cmd;         // Соответствующая команда
    };

    Command parse_command(const char* input) {
        const Keyword keywords[] = {
        {"назад",  Command::BACKWARD},  
        {"выход",  Command::EXIT},
        {"вперёд", Command::FORWARD},  
        {"налево", Command::LEFT},
        {"направо",Command::RIGHT}   
        };
        for (const auto& keyword : keywords) {
            // Полное сравнение строк
            if (strcmp(input, keyword.word) == 0) {
                return keyword.cmd;
            }
        }
        return Command::UNKNOWN;
    }

    class Maze
    {
    public:
        Maze(unsigned n) : size(n),
                           h_wall(n + 1, std::vector<bool>(n, true)),
                           v_wall(n, std::vector<bool>(n + 1, true))
        {
            // Инициализация игрока и выхода
            player_x = 0;
            player_y = 0;
            exit_x = n - 1;
            exit_y = n - 1;
            generateMaze();
        }

        bool playerWin() {
            if (player_x == exit_x && player_y == exit_y) {
                return true;
            }
            return false;
        } 

        // Получить ASCII-рисунок лабиринта с позицией игрока 'P' и выходом 'E'
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

        // Попытка движения: dx,dy = {0,-1}(вверх), {0,1}(вниз), {1,0}(вправо), {-1,0}(влево)
        bool movePlayer(int dx, int dy)
        {
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
        unsigned size;
        std::vector<std::vector<bool>> h_wall; // горизонтальные стены (size+1 x size)
        std::vector<std::vector<bool>> v_wall; // вертикальные стены (size x size+1)
        int player_x, player_y;
        int exit_x, exit_y;

        // Рекурсивная генерация лабиринта (DFS) для связности
        void generateMaze()
        {
            std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
            carve(0, 0, visited);
        }

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
