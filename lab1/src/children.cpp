#include <iostream>
#include <fcntl.h>
#include <unistd.h> 
#include <vector>
#include <stdio.h>
#include <sys/wait.h>
#include <sstream>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: <pipe_read_fd> <output_file>" << std::endl;
        return -1;
    }

    // Получаем файловый дескриптор для чтения
    int pipe_read_fd = atoi(argv[1]);

    int file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (file == -1) {
        std::cerr << "Failed to open/create file\n";
        return -1;
    }

    // Считываем данные из pipe
    char buffer[256];
    ssize_t bytes_read = read(pipe_read_fd, buffer, sizeof(buffer));
    close(pipe_read_fd);

    if (bytes_read < 0) {
        std::cerr << "Failed to read from pipe\n";
        close(file);
        return -1;
    }

    buffer[bytes_read] = '\0';
    std::istringstream iss(buffer);
    std::vector<int> numbers;
    int num;

    while (iss >> num) {
        numbers.push_back(num);
    }

    if (numbers.size() < 2) {
        std::cerr << "Not enough numbers to divide\n";
        close(file);
        return -1;
    }

    int value = numbers[0];

    for(size_t i = 1; i < numbers.size(); ++i) {
        if (numbers[i] == 0) {
            std::cerr << "Error: division by zero\n";
            close(file);
            return -1;
        }

        value /= numbers[i];
    }

    dprintf(file, "%d", value);
    close(file);
}