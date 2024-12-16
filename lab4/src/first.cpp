#include "../include/prime_naive.cpp"
#include "../include/bubble_sort.cpp"

#include <iostream>

int main() {
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        int A, B;
        std::cin >> A >> B;
        std::cout << PrimeCount(A, B) << std::endl;
    } else if (choice == 2) {
        int size;
        std::cin >> size;

        int* arr = new int[size];
        
        for (int i = 0; i < size; ++i) {
            std::cin >> arr[i]; 
        }
        
        Sort(arr, size);
        
        for (int i = 0; i < size; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
        delete[] arr;
    } else {
        std::cout << "Invalid choice!\n";
    }
}