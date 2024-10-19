#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <sys/sysinfo.h>

// Мьютекс для синхронизации доступа
pthread_mutex_t mutex;

uint32_t MAX_THREADS; // Макс кол-во потоков
uint32_t ACTIVE_THREADS = 0; // Кол-во активных потоков

// Конфиг для передачи аргументов в другие потоки
struct ThreadConfig {
    int* arr;
    int left;
    int right;
};

int partition(int* arr, int left, int right) {
    int pivot = arr[right]; // Выбираем опорный элемент
    int i = left - 1;

    // Элементы меньшие или равные опорному размещаем слева,
    // а большие - справа
    for (int j = left; j < right; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[right]);
    return i + 1; // Возврашщаем опорный элемент
}

void* quicksort(void *args) {
    std::cout << "ACTIVE THREADS: " << ACTIVE_THREADS << std::endl;

    // Получаем аргументы путем приведения к ThreadConfig*
    ThreadConfig *targs = (ThreadConfig*)args;

    int* arr = targs->arr;
    int left = targs->left;
    int right = targs->right;

    if (left < right) {
        int pivot = partition(arr, left, right);

        // Конфиги для передачи в потоки
        ThreadConfig left_cfg = {arr, left, pivot - 1};
        ThreadConfig right_cfg = {arr, pivot + 1, right};

        pthread_t left_t, right_t;
        bool create_left_thread = false, create_right_thread = false;

        // Для левой части
        pthread_mutex_lock(&mutex);
        if (ACTIVE_THREADS < MAX_THREADS) {
            ++ACTIVE_THREADS;
            create_left_thread = true;
        }
        pthread_mutex_unlock(&mutex);

        // Создаем новый поток если можем, если нет - запускаем в этом же
        if (create_left_thread) {
            pthread_create(&left_t, nullptr, quicksort, &left_cfg);
        } else {
            quicksort(&left_cfg);
        }

        // Ананлогично для правой части
        pthread_mutex_lock(&mutex);
        if (ACTIVE_THREADS < MAX_THREADS) {
            ++ACTIVE_THREADS;
            create_right_thread = true;
        }
        pthread_mutex_unlock(&mutex);

        if (create_right_thread) {
            pthread_create(&right_t, nullptr, quicksort, &right_cfg);
        } else {
            quicksort(&right_cfg);
        }

        // Ждем завершения потоков
        if (create_left_thread) pthread_join(left_t, nullptr);
        if (create_right_thread) pthread_join(right_t, nullptr);

        // Уменьшаем кол-во активных потоков с блокировкой мьютекса
        pthread_mutex_lock(&mutex);
        --ACTIVE_THREADS;
        pthread_mutex_unlock(&mutex);
    }

    return nullptr;
}

void parallel_quicksort(int* arr, int left, int right) {
    ThreadConfig cfg = {arr, left, right};
    quicksort(&cfg);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <max_threads>" << std::endl;
        return -1;
    }

    MAX_THREADS = std::atoi(argv[1]); // Получаем значение макс-ого кол-ва потоков
    pthread_mutex_init(&mutex, NULL); // Инициализируем мьютекс

    int n;
    std::cout << "Enter the size of array: ";
    std::cin >> n;

    std::vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    // Сортируем
    parallel_quicksort(arr.data(), 0, arr.size() - 1);

    // Выводим результат
    std::cout << "Sorted array: \n";
    for (int num : arr) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}