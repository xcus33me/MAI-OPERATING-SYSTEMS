#!/bin/bash

# Входные данные для нахождения кол-ва простых чисел
input1="1 30 3453"

# Входные данные для сортировки массива
input2="2 10 128 39 17 49 2672 58 37 28 192 38"

# Запуск нахождения кол-ва простых чисел
echo "$input1" | ./first
echo "$input1" | ./second

# Запуск сортировки
echo "$input2" | ./first
echo "$input2" | ./second
