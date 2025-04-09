#!/bin/bash

PROG="build/oscillation_main"

TIME=1000
STEP=0.001

THREADS=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)

OUT="result.txt"
PLOT_OUT="res_diagram.gp"

echo "Threads Time(ms)" > $OUT

for i in "${THREADS[@]}"; do
	$PROG -t $TIME -x $STEP -n $i -s 2>&1 |\
	grep "Threads:" |\
	awk '{print $2, $4}' >> $OUT
done

cat $OUT
gnuplot $PLOT_OUT
