## Лабораторная работа №1 по дисциплине "Разработка программных систем" (кафедра РК6).
## Скрипт создания GIF-анимации колебаний струны для gnuplot.
## Необходима версия gnuplot 5 и выше.
## Запуск: gnuplot -c create_animation.gp [data_file_name] [output_file_name.gif]
## Автор: Козов А.В.

set terminal gif animate size 1024,512 font "sans,14" # delay 10
## Параметр delay задаёт время между кадрами в 0,01 с, значение по умолчанию 5.

datafile = './build/data.txt'
outfile = 'oscillation.gif'

## Передача аргументов работает только при вызове с ключом -с (см. команду запуска выше)!
if (ARGC > 0) {
  datafile = ARG1
}
if (ARGC > 1) {
  outfile = ARG2
}

unset key                                               ## отключение легенды
set xlabel '{/:Italic x}'                               ## название горизонтальной оси
set ylabel '{/:Italic u} ({/:Italic x}, {/:Italic t} )' ## название вертикальной оси

set output outfile
stats datafile nooutput
set decimalsign ','
set grid

set xrange [STATS_min_x:STATS_max_x]
set yrange [STATS_min_y:STATS_max_y]

do for [i = 0:STATS_blocks-1] {
  plot datafile index i with lines lw 2 lc rgb "navy"
}
