#!/bin/bash

g++ -o parent src/parent.cpp
g++ -o child src/child.cpp

if [ $? -eq 0 ]; then
    ./parent
else
    echo "Ошибка компиляции."
fi