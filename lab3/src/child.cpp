#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file_path> <int FILESIZE>\n", argv[0]);
        return 1;
    }

    // Получаем путь к файлу. размер
    char* filepath = argv[1];
    int FILESIZE = atoi(argv[2]);

    // Открываем файл для чтения и записи
    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        perror("Error opening file in child.");
        return 1;
    }

    // Отображает в память содерживое файла, разрешает запись и чтение
    char *mapped = (char*)mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("Error mapping file in child.");
        close(fd);
        return 1;
    }
    close(fd);

    // Парсинг чисел из memory-mapped файла
    int nums[128];
    int count = 0;
    char *token = strtok(mapped, " ");
    while (token != NULL) {
        nums[count++] = atoi(token);
        token = strtok(NULL, " ");
    }

    if (count < 2) {
        fprintf(stderr, "Insufficient data to perform operation.\n");
        munmap(mapped, FILESIZE);
        return 1;
    }

    // Выполняем последовательное деление
    int result = nums[0];
    for (int i = 1; i < count; ++i) {
        if (nums[i] == 0) {
            fprintf(stderr, "Division by 0. Exiting.\n");
            munmap(mapped, FILESIZE);
            kill(getppid(), SIGKILL); // Сообщаем родителю о завершении
            return 1;
        }
        result /= nums[i];
    }

    // Записываем финальный результат в файл
    FILE *result_file = fopen("result.txt", "w"); // Открываем в режиме записи, чтобы перезаписать содержимое
    if (!result_file) {
        perror("Error opening result file.");
        munmap(mapped, FILESIZE);
        return 1;
    }

    fprintf(result_file, "%d", result);
    fclose(result_file);

    // Освобождаем ресурсы
    munmap(mapped, FILESIZE);
    return 0;
}