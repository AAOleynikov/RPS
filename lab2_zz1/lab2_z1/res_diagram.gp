# Настройки стиля
set terminal pngcairo size 800,600
set output "res_diagram.png"
set title "Результаты выполнения"
set xlabel "Номер эксперимента"
set ylabel "Время, сек"
set style fill solid
set boxwidth 0.8
set key off

# Автоматическая нумерация по X (начиная с 1)
plot "result.txt" using 1:2:xtic(1) with boxes lc rgb "#F5BB98" notitle