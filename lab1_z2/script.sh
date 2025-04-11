#!/bin/bash

PROG="build/oscillation_main"

TIME=100
STEP=0.01

THREADS=(1 2 4 8)

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
