#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

//g++ -o wunzip wunzip.cpp -Wall -Werror
using namespace std;

void wunzip(const char* filename) {
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0) {
        const char* errorMsg = "wunzip: cannot open file ";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        write(STDERR_FILENO, filename, strlen(filename));
        const char* newline = "\n";
        write(STDERR_FILENO, newline, strlen(newline));
        exit(1);
    }

    int count;
    char character;
    ssize_t bytesRead;

    while ((bytesRead = read(fileDescriptor, &count, sizeof(count))) > 0) {
        if (read(fileDescriptor, &character, sizeof(character)) < 0) {
            const char* errorMsg = "wunzip: error reading file ";
            write(STDERR_FILENO, errorMsg, strlen(errorMsg));
            write(STDERR_FILENO, filename, strlen(filename));
            const char* newline = "\n";
            write(STDERR_FILENO, newline, strlen(newline));
            exit(1);
        }
        for (int i = 0; i < count; ++i) {
            write(STDOUT_FILENO, &character, sizeof(character));
        }
    }

    if (bytesRead < 0) {
        const char* errorMsg = "wunzip: error reading file ";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        write(STDERR_FILENO, filename, strlen(filename));
        const char* newline = "\n";
        write(STDERR_FILENO, newline, strlen(newline));
        exit(1);
    }

    close(fileDescriptor);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        const char* errorMsg = "wunzip: file1 [file2 ...]\n";
        write(STDERR_FILENO, errorMsg, strlen(errorMsg));
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        wunzip(argv[i]);
    }

    return 0;
}
