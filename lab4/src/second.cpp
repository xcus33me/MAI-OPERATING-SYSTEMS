#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <dlfcn.h>

// Указатели на функции
int (*PrimeCountFunc)(int, int);
int* (*SortFunc)(int*, int);

// Указатели на библиотеки
void* handle_prime;
void* handle_sort;

void load_libs();

int main() {
    load_libs();

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        int A, B;
        std::cin >> A >> B;
        std::cout << (*PrimeCountFunc)(A, B) << std::endl;
    } else if (choice == 2) {
        int size;
        std::cin >> size;

        int* arr = new int[size];
        
        for (int i = 0; i < size; ++i) {
            std::cin >> arr[i]; 
        }
        
        (*SortFunc)(arr, size);
        
        for (int i = 0; i < size; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
        delete[] arr;
    } else {
        std::cout << "Invalid choice!\n";
    }

    dlclose(handle_prime);
    dlclose(handle_sort);
}

void load_libs() {
    // Загрузка библиотек
    handle_prime = dlopen("libs/prime_sieve.so", RTLD_LAZY);
    handle_sort = dlopen("libs/hoare_sort.so", RTLD_LAZY);


    if (!handle_prime) {
        fprintf(stderr, "Error loading prime library: %s\n", dlerror());
        exit(1);
    }
    if (!handle_sort) {
        fprintf(stderr, "Error loading sort library: %s\n", dlerror());
        exit(1);
    }

    // Reset dlerror()
    dlerror();

    // Получение символа "PrimeCount" из библиотеки. Указатель на функцию извлекается из библиотеки
    PrimeCountFunc = (int (*)(int, int))dlsym(handle_prime, "PrimeCount");
    char* error = dlerror();
    if (error) {
        fprintf(stderr, "Error getting PrimeCount symbol: %s\n", error);
        exit(1);
    }

    // Получение символа "Sort" из библиотеки. Указатель на функцию извлекается из библиотеки   
    SortFunc = (int* (*)(int*, int))dlsym(handle_sort, "Sort");
    error = dlerror();
    if (error) {
        fprintf(stderr, "Error getting Sort symbol: %s\n", error);
        exit(1);
    }
}