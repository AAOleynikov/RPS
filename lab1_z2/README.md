# Комманды для запуска
```mkdir build``` - создаём директорию, в которой будем компилировать
```cd build``` - переходим в созданную директорию
```cmake ..``` - основываясь на заголовочном файле CMakeLists.txt инициализируем компиляторы и тп.
```cmake --build .``` - собираем проект и получаем исполняемый файл ./oscillation_main который мы можем запускать с флагами (описаны далее)
```./oscillation_main -t 100 -x 0.01 -n 1``` - параметры, которые задал Козов
В результате запуска, если не был включн тестовый режим (ключ -s) будес сформирован файлик data.txt, по которому мы сформируем гифку
```cd ..``` - переходим на 1 уровень выше по директории
```gnuplot -c create_animation.gp``` - формирование гифки oscillation.gif
```./script.sh``` - прогонка полученной программы для 1 2 4 8 потоков, получится текстовый файл result.txt в котором время прогонки, а также диаграмма plot.png визуализирующая
Вы должны получить тренд уменьшения времени исполнения если у вас 8 физических ядер, если 4, то 1 2 4 будет уменьшение и на 8 - рост (если у вас есть гипертейдинг), если же у вас менее 8 логических ядер - в файле script.sh поменяйте то, для скольких потоков у вас будет прогонка 

# Лабораторная работа №1
Лабораторная работа №1 (3 академических часа) направлена на получение практических навыков разработки программных систем с использованием многопоточности для повышения производительности.

## Задание на лабораторную работу №1
Необходимо разработать приложение для моделирования колебаний однородной упругой струны и провести моделирование согласно заданиям.

### Требования к разрабатываемой программной системе
Разрабатываемая программная система должна обеспечивать моделирование свободных и вынужденных колебаний однородной упругой горизонтальной струны методом конечных разностей (МКР) с явной вычислительной схемой.
Длина струны - 1 м, скорость распространения волны (фазовая скорость) - 1 м/с.
Пользователь при запуске приложения задаёт шаг дискретизации пространства (в метрах), время моделирования (в секундах), а также число потоков выполнения.
При запуске в многопоточном режиме приложение должно обеспечивать _ускорение вычислений_.
Начальные и граничные (первого и второго рода) условия допустимо задавать в коде программы.

Результат моделирования необходимо представить в графическом виде (GIF-анимация колебаний струны) при помощи утилиты gnuplot.

Для проверки ускорения вычислений следует предусмотреть специальный режим работы _без файлового вывода_.

### Задание 1
При помощи разработанной программной системы провести моделирование свободных колебаний струны с жёстким закреплением обоих концов при ненулевых начальных условиях.

Получить визуализацию колебаний для времени моделирования 10 с с шагом дискретизации пространства 0,01 м.

Выполнить замеры времени выполнения расчётов для числа потоков 1, 2, 4, 8 при решении _большой_ задачи (время решения для одного потока - не менее 100 с) в служебном режиме.
Построить полученную зависимость времени вычислений от числа потоков, объяснить результаты.

### Задание 2
При помощи разработанной программной системы провести моделирование вынужденных колебаний струны при нулевых начальных условиях.
Левый конец струны жёстко закреплён, правый конец струны - свободный.
Внешняя сила приложена к середине струны и изменяется по гармоническому закону.

Получить визуализацию колебаний для времени моделирования 10 с с шагом дискретизации пространства 0,01 м.

Выполнить замеры времени выполнения расчётов для числа потоков 1, 2, 4, 8 при решении _большой_ задачи (время решения для одного потока - не менее 100 с) в служебном режиме.
Построить полученную зависимость времени вычислений от числа потоков, объяснить результаты.

## Математическое описание задачи
Малые поперечные колебания $` u(x, t) `$ однородной упругой струны под действием внешних сил описывает уравнение струны (частный случай волнового уравнения)
```math
  \frac{\partial^2 u}{\partial t^2} = a^2 \frac{\partial^2 u}{\partial x^2} + p(x, t),
```
где $` a `$ - постоянная фазовая скорость (зависит от материала и натяжения),
$` p(x, t) `$ - внешняя сила (может меняться в разных точках струны и во времени).
Если внешняя сила отсутствует ($` p(x, t) = 0 `$), то это уравнение описывает свободные колебания струны.

К точке струны могут быть приложены граничные условия (ГУ):
- первого рода (задано значение фазовой переменной $` u = C_{1} `$);
- второго рода (задано значение первой производной фазовой переменной по пространственной координате $` \frac{\partial u}{\partial x} = C_{2} `$);
- третьего рода (задано отношение значения фазовой переменной и её первой производной по пространственной координате).

В контексте рассматриваемой задачи ГУ первого рода означает заданное пользователем положение точки струны (например, жёсткое закрепление).
ГУ второго рода задаёт скорость изменения положения точки относительно соседних точек, $` \frac{\partial u}{\partial x} = 0 `$ означает свободное перемещение.

### Конечно-разностная аппроксимация
При использовании МКР производные заменяются своим разностным аналогом, например, по пространственной координате
```math
  \frac{\partial u}{\partial x} \approx \frac{u_{i+1} - u_{i}}{\Delta x},
```
где $` i `$ - номер узла равномерной сетки с шагом дискретизации $` \Delta x = x_{i+1} - x_{i} `$.
Аналогичным образом можно дискретизировать время моделирования и получить шаг по времени $` \Delta t = t^{j+1} - t^j `$.

Определим вторую производную в $` i `$-м узле сетки на $` j `$-временном шаге ("слое"):
```math
  \frac{\partial^2 u}{\partial x^2} \approx \frac{u^j_{i+1} - 2 u^j_{i} + u^j_{i-1}}{\Delta x^2}.
```

Использование явной вычислительной схемы предполагает расчёт значений фазовой переменной $` u^{j+1}_i `$ в $` i `$-ом узле на временном шаге  $` j+1 `$ по значениям с предыдущего шага $` j `$ и внешней силы $` p^{j+1}_i `$:
```math
  \frac{u^{j+1}_{i} - 2 u^j_{i} + u^{j-1}_{i}}{\Delta t^2} = a^2 \frac{u^j_{i+1} - 2 u^j_{i} + u^j_{i-1}}{\Delta x^2} + p^{j+1}_i.
```
С учётом того, что значения $` u^j_i `$, $` u^{j-1}_i `$, $` u^j_{i-1} `$, $` u^j_{i+1} `$ известны с предыдущих шагов моделирования и начальных условий, запишем формулу для нового значения фазовой переменной
```math
  u^{j+1}_{i} = 2 u^j_{i} - u^{j-1}_{i} + (a^2 \frac{u^j_{i+1} - 2 u^j_{i} + u^j_{i-1}}{\Delta x^2} + p^{j+1}_i) \Delta t^2 .
```
Если для узла $` i' `$ задано ГУ первого рода, то значение фазовой переменной равно заданному значению:
```math
  u^{j+1}_{i'} = C_{1}.
```
Если для узла $` i'' `$, находящегося на конце струны, задано ГУ второго рода, то для сохранения порядка точности необходимо ввести фиктивный узел и аппроксимировать первую производную при помощи центральной разности.
Значения в узле $` i'' `$ рассчитывается по основной формуле, в фиктивном - по формуле центральной разности для первой производной.

Пусть ГУ второго рода задано для крайнего правого узла с номером $` N `$.
Введём фиктивный узел $` N + 1 `$ и запишем выражение для ГУ второго рода
```math
  \frac{u^{j+1}_{N-1} - u^{j+1}_{N+1}}{2\Delta x} = C_{2}.
```
Выразим $` u^{j+1}_{N+1} `$
```math
  u^{j+1}_{N+1} = u^{j+1}_{N-1} - C_{2} 2\Delta x
```
и подставим в разностное уравнение для узла $` N `$:
```math
  u^{j+1}_{N} = 2 u^j_{N} - u^{j-1}_{N} + (a^2 \frac{(u^{j+1}_{N-1} - C_{2} 2\Delta x) - 2 u^j_{N} + u^j_{N-1}}{\Delta x^2} + p^{j+1}_N) \Delta t^2.
```

### Устойчивость вычислительной схемы
Явная вычислительная схема может быть неустойчивой (невязка катастрофически растёт при увеличении времени моделирования).
Для обеспечения устойчивости необходимо соблюдение условия $` a \Delta t / \Delta x \le 1 `$, т.е. при $` a = 1 `$ шаг дискретизации по временной координате должен быть _не больше_ шага дискретизации по пространственной координате.

Пусть $` a \Delta t / \Delta x = 0,1 `$, тогда $` \Delta t = 0,1 \Delta x `$.

### Постановка задачи
Заданы следующие исходные данные:
- длина струны $` L = 1~м `$;
- фазовая скорость $` a = 1~м/с `$;
- шаг пространственной дискретизации $` \Delta x~м`$;
- продолжительность (время) моделирования $` D~с`$;
- число шагов временной дискретизации $` T = D / \Delta t = D / (0,1 \Delta x) `$;
- начальные условия (координаты узлов струны в начальный момент времени) $` u^0_i~м `$, $` i = 0, 1, \dots, N`$, где $` N `$ - число интервалов дискретизации, $` N = L / \Delta x `$;
- граничные условия $` C_1 `$, $` C_2 `$ (только для узлов с номерами $` 0 `$ и $` N `$).

Необходимо определить значения $` u^j_i `$ для $` i = 0, 1, \dots, N `$ и $` j = 1, 2, \dots, T `$.

## Программная реализация

### Декомпозиция программной системы
Разработаем приложение как программную систему из двух компонентов: основной программы на C++, которая выполняет моделирование, и вспомогательного скрипта визуализации.
Основная программа запускается из командной строки (терминала) и формирует файл данных, содержащий искомые значения фазовой переменной с разбивкой по временным слоям.
Скрип визуализации использует файл данных для формирования GIF-анимации при помощи утилиты gnuplot.

Такая реализация позволяет, например, запускать основную программу на удалённом сервере и по скаченному файлу данных локально запускать скрипт визуализации.

Для реализации основной программы воспользуемся средствами многопоточного программирования C++11.

Для сборки используем CMake, где пропишем дополнительную зависимость - утилиту gnuplot.
При её отсутствии CMake будет выдавать пользователю предупреждение.

### Основная программа
Воспользуемся аргументами запуска для того, чтобы обеспечить возможность задать параметры моделирования:
- `-t duration` - время моделирования, положительное вещественное число (по умолчанию 1 с);
- `-x x_step` - шаг пространственной дискретизации, положительное вещественное число  (по умолчанию 0,05 м);
- `-n threads` - число потоков выполнения, положительное целое число (по умолчанию 1);
- `-o file_name` - название файла данных для записи (по умолчанию "data.txt");
- `-s` - служебный режим для замера времени вычислений.

Например, запуск основной программы `./oscillation_main` с ключами `-t 5.5 -n 4 -o data_1.txt` сформирует фай данных `data_1.txt` для моделирования колебаний в течение 5,5 с при расчёте с использованием четырёх потоков с шагом по умолчанию 0,05 м.

При недопустимых значениях аргументов завершим работу с ненулевым кодом возврата.

#### Декомпозиция на классы
Выделим следующие классы основной программы:
- `StringOscillation` - основной класс, реализующий моделирование;
- `StringSolution` - класс с массивом значений фазовой переменной для одного временного слоя;
- `AbstractStringSolver` - абстрактный класс, определяющий интерфейс для реализации способов решения уравнения струны;
- `StringPlotter` - класс для формирования файла данных для gnuplot;
- `StringParameters` - вспомогательная структура, задающая все параметры моделирования;
- `ParameterParser` - вспомогательный класс для разбора аргументов запуска и формирования параметров;
- `SingleThreadStringSolver` - отладочный класс однопоточной реализации МКР для решения уравнения струны с заданными параметрами, наследует `AbstractStringSolver`.
- `MultiThreadStringSolver` - класс многопоточной реализации МКР для решения уравнения струны с заданными параметрами, наследует `AbstractStringSolver`.

_Примечание 1_. Согласно принципам "чистой" архитектуры вместо класса `StringPlotter` следовало бы использовать абстрактный класс с наследниками для формирования файлов в разных форматах (gnuplot, csv, файлы для визуализации на python и пр.).
Аналогичным образом следовало бы поступить с классом `ParameterParser` и связать конкретную реализацию абстрактного класса с реализацией метода решения.

_Примечание 2_. Часть `String` в названиях классов можно опустить, если поместить все классы в пространство имён `string`.

#### Алгоритм основной функции (упрощённый)
1. Создание объекта `ParameterParser` на основе аргументов основной функции.
2. Создание объекта `StringParameters` при помощи вызова метода `getParameters` от объекта `ParameterParser`.
3. Создание объекта `StringOscillation`.
4. Вызов основного метода `simulate` с ранее созданным объектом `StringOscillation` в качестве аргумента.
5. Обработка исключений.

### Скрипт визуализации
Создадим скрипт утилиты gnuplot для визуализации результатов в виде GIF-анимации.

#### Базовый вывод
Базовые настройки для задания способа вывода в виде GIF-анимации выглядят следующим образом:
```gnuplot
set terminal gif animate delay 10

set output 'oscillation.gif' ## название результирующего файла

unset key                    ## отключение легенды
set xlabel 'x'               ## название горизонтальной оси
set ylabel 'u(x,t)'          ## название вертикальной оси
```

Для отрисовки струны на одном кадре анимации воспользуемся стандартной командой gnuplot `plot with lines`.
Она требует, чтобы данные в файле были записаны построчно, в виде пар "x u", где x - значение пространственной переменной, u - значение фазовой переменной:
```
0 0
0.01 0.01
0.02 0.05
0.03 0.12
# строки, начинающиеся с решётки, будут проигнорированы
```

Зададим большую толщину линии для отрисовки и цвет: `plot with lines lw 3 lc rgb "navy"`.

#### Создание GIF-анимации колебаний
Для создания анимации колебаний струны необходимо последовательно формировать "кадры", на каждом из которых представлено состояние струны для одного временного слоя.
При использовании одного файла данных gnuplot требует, чтобы значения для каждого кадра были разделены двойным переводом строки.
В таком случае в команду `plot` можно добавить `index i`, чтобы указать, какой набор из разделённых двойным переводом строки данных использовать для отрисовки, а саму команду следует заключить в цикл по переменной `i`:
```gnuplot
do for [i=0:99] { ## для 100 кадров анимации
  plot 'data.dat' index i with lines lw 2 lc rgb "navy"
}
```

Основной проблемой получившегося скрипта является то, что gnuplot на каждом кадре будет подстраивать диапазоны значений по вертикальной и горизонтальной осям под максимальное и минимальное значение для текущего временного слоя.
В результате положение струны на последовательности кадров анимации будет "скакать" вверх-вниз.
Ограничить диапазоны значений можно при помощи следующий команд:
```gnuplot
set xrange [0:1]  # значения по горизонтальной оси меняется от 0 до 1
set yrange [-1:1] # значения по вертикальной оси меняется от -1 до 1
```

#### Параметризация скрипта
Организуем передачу названий файлов данных и анимации через аргумент вызова gnuplot (начиная с версии 5), чтобы не привязывать работу скрипта к единственно возможному названию:
```gnuplot
datafile = 'data.txt'
outfile = 'oscillation.gif'

if (ARGC > 0) {
  datafile = ARG1
}
if (ARGC > 1) {
  outfile = ARG2
}
```

Для получения аргументов вызова наш скрипт потребуется передавать утилите gnuplot с ключом `-c` (см. пример в конце).

Для повышения гибкости и автоматической "подстройки" диапазона значений на максимальное и минимальное среди полученных при моделировании следует использовать команду `stats`.
Эта команда которая позволяет проанализировать файл, название которого необходимо передать в качестве аргумента.
В результате выполнения команды `stats datafile` статистика файла данных будет выведена на экран, а в скрипте станут доступны специальные переменные с префиксом STATS_, например, `STATS_min_x`.
Чтобы предотвратить лишний вывод, используем дополнительный аргумент `nooutput` для `stats`.

Настройка отображения с использованием статистики будет выглядеть следующим образом:
```gnuplot
stats datafile nooutput
set xrange [STATS_min_x:STATS_max_x]
set yrange [STATS_min_y:STATS_max_y]

do for [i = 0:STATS_blocks-1] {
  plot datafile index i with lines lw 2 lc rgb "navy"
}
```

#### Финальные штрихи
По умолчанию gnuplot создаёт анимацию с разрешением 640 на 480 пикселов.
Для увеличения качества изображения и размера шрифта дополним строку со способом вывода следующим образом: `set terminal gif animate delay 20 size 1024,512 font "sans,20"`.

Для использования в качестве разделителя целой и дробной части запятой воспользуемся командой `set decimalsign ','`.

Включим отображение координатной сетки для лучшей читаемости: `set grid`.

Зададим курсивное начертание для латинских обозначений в названии осей при помощи атрибутов шрифта:
```gnuplot
set xlabel '{/:Italic x}'
set ylabel '{/:Italic u} ({/:Italic x}, {/:Italic t} )'
```

Пример запуска скрипта визуализации:
```bash
gnuplot -c create_animation.gp example_data.txt animation.gif
```

В результате gnuplot попытается считать данные из файла `example_data.txt` и создать GIF-анимацию `animation.gif`.
