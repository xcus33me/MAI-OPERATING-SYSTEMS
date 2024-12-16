#!/bin/bash

g++ -pthread -o sort src/main.cpp

if [ $? -eq 0 ]; then
    ./sort 4
else
    echo "Ошибка компиляции."
fi