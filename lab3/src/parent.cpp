#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define FILEPATH "/tmp/file"
#define FILESIZE 4096

pid_t child_pid;

void signal_handler(int sig) {
    // Обработчик для дочернего процесса, ничего не делает
}

int main() {
    int fd; // Файловый дескриптор для работы с файлом
    char* mapped; // Указатель на отображенную облясть памяти

    // Создание или открытие отображаемого файла в папке /tmp; 
    // Если нет файла - создаем; Доабавляем права для чтения и записи;
    fd = open(FILEPATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening file.");
        exit(1);
    }

    // Устанавливаем размер файла
    if (ftruncate(fd, FILESIZE) == -1) {
        perror("Error setting file size.");
        close(fd);
        exit(1);
    }

    // Отображаем файл. Разрешаем чтение, запись.
    mapped = (char*)mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("Error mapping file.");
        close(fd);
        exit(1);
    }
    close(fd); // Закрываем файловый дескриптор

    // Создаём дочерний процесс
    child_pid = fork();
    if (child_pid == -1) {
        perror("Fork failed.");
        munmap(mapped, FILESIZE);
        exit(1);
    } else if (child_pid == 0) {
        // Дочерний процесс

        // Устанавливаем оброботчик сигнала
        signal(SIGUSR1, signal_handler);
        pause();

        // После получения сигнала запускает новую программу
        char filesize_str[10];
        sprintf(filesize_str, "%d", FILESIZE);
        const char* args[] = {"./child", FILEPATH, filesize_str, NULL};

        execve("./child", (char* const*)args, NULL); 
        perror("execve failed.");
        exit(1);
    } else {
        // Родительский процесс
        std::string numbers;
        std::cout << "Enter numbers separated by space (e.g., '10 2 5'): ";
        std::getline(std::cin, numbers);

        if (numbers.size() >= FILESIZE) {
            perror("Data exceeds file size limit");
            munmap(mapped, FILESIZE);
            unlink(FILEPATH);
            exit(1);
        }

        // Копируем данные в отображенную память
        memcpy(mapped, numbers.c_str(), numbers.size() + 1);

        // Посылает сигнал дочернему процессу
        kill(child_pid, SIGUSR1);

        // Ожидаем завершения дочернего процесса
        int status;
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Child process exited with code: " << WEXITSTATUS(status) << "\n";
        } else if (WIFSIGNALED(status)) {
            std::cout << "Child process terminated by signal: " << WTERMSIG(status) << "\n";
        }

        // Освобождаем память
        munmap(mapped, FILESIZE);
        unlink(FILEPATH);
    }

    return 0;
}
