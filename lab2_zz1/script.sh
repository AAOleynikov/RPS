#!/bin/bash

PROG="build/oscillation_mpi_main"

TIME=10
STEP=0.001

THREADS=(1 2 3 4 5 6 7 8)

OUT="result.txt"
PLOT_OUT="res_diagram.gp"

# Очищаем файл результатов
> $OUT

for i in "${THREADS[@]}"; do
    # Записываем номер потоков и результат в одну строку
    echo -n "$i " >> $OUT
    mpiexec -n $i -f ~/.machinefile ./$PROG -t $TIME -x $STEP -s >> $OUT
done

cat $OUT
gnuplot $PLOT_OUT