#!/bin/bash

# Массив для хранения PID клиентов
CLIENT_PIDS=()

# Завершение всех клиентов при прерывании
trap 'kill -TERM "${CLIENT_PIDS[@]}" 2>/dev/null; wait "${CLIENT_PIDS[@]}"' SIGINT

# Запуск клиентов
for i in {1..100}; do
    ( 
        ./build/client -t -p 9001 2>&1 | 
        awk -v id="$i" '{print "[CLIENT " id "] " $0}' 
    ) &
    CLIENT_PIDS+=($!)
done

# Ожидание завершения
wait "${CLIENT_PIDS[@]}"