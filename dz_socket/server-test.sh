#!/bin/bash

if [[ $# -lt 1 ]]; then
  echo "no client path!"
  exit 1
fi
COUNTER=0
while [[ COUNTER -lt 200 ]]; do
  ./$1 -n 100 < Doxyfile > /dev/null &
  let COUNTER++
done
echo "finish"
