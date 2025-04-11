set terminal pngcairo size 800,600
set output "plot.png"
set title "Зависимость времени выполнения от числа потоков"
set xlabel "Число потоков"
set ylabel "Время (мс)"
set style fill solid
set boxwidth 0.8
set auto x
plot "result.txt" using 0:2:xtic(1) with boxes lc rgb "#F5BB98" notitle
