#include <cmath>
#include <iostream>

// Подсчёт количества простых чисел на отрезке [A, B] (A, B - натуральные);
// 1 реализация - Наивный алгоритм. Проверить делимость текущего числа на все предыдущие числа.
extern "C" {
    bool isPrime(int n) {
        if (n <= 1) return false;
        if (n == 2) return true; // 2 - единственное четное простое число
        if (n % 2 == 0) return false; // Остальные четные числа не простые
    
        // Проверяем делители от 3 до sqrt(n)
        for (int i = 3; i <= std::sqrt(n); i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
    
    int PrimeCount(int A, int B) {
        if (B < 2) return 0;  // Нет простых чисел ниже 2.
        if (A < 2) A = 2;    // Начинаем с минимального простого числа
    
        int count = 0;
        for (int num = A; num <= B; num++) {
            if (isPrime(num)) {
                ++count;
            }
        }
        return count;
    }
}