#include <iostream>
#include <stdio.h>
#include <unistd.h> 
#include <vector>
#include <sys/wait.h>
#include <string.h>
#include <sstream>

int main() {
    // Создаем канал для передачи данных между процессами
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        std::cerr << "Failed to create pipe\n";
        return -1;
    }
    
    // Считываем название файла
    std::string file_name;
    std::cout << "Enter file name: ";
    std::getline(std::cin, file_name);

    // Создаем дочерний процесс
    pid = fork();
    if (pid < 0) {
        std::cerr << "Error creating child process\n";
        return -1;
    }

    if (pid == 0) {
        // Дочерний процесс
        close(fd[1]); // Закрываем канал для записи

        // Сохраняем в массивах char файловый дескриптор и название выходного файла
        char pipe_read_fd[10];
        char file_name_arg[256];

        strcpy(file_name_arg, file_name.c_str());
        sprintf(pipe_read_fd, "%d", fd[0]);

        // Сохраняем аргументы
        char* args[] = {const_cast<char*>("./child"), pipe_read_fd, file_name_arg, NULL};

        // Заменяем процесс на выполнение дочерней программы child
        execve("./child", args, NULL);
        exit(1);

    } else {
        close(fd[0]); // Закрываем канал для чтения

        // Ввод чисел
        std::string numbers;
        std::cout << "Enter numbers separated by space (e.g., '10 2 5'): ";
        std::getline(std::cin, numbers);
        
        // Записываем данные 
        write(fd[1], numbers.c_str(), numbers.size());
        close(fd[1]); // Закрываем канал для записи

        wait(NULL); // Ожидаем завершения дочернего потока
    }

    return 0;
}