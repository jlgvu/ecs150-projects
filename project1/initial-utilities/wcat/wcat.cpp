#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

void printFile(const char* filename) {
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0) {
        cerr << "wcat: cannot open file" << endl;
        exit(1);
    }

    char buffer[4096]; // Buffer to read from file
    ssize_t bytesRead;

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead) {
            cerr << "wcat: error writing to stdout" << endl;
            exit(1);
        }
    }

    if (bytesRead < 0) {
        cerr << "wcat: error reading from file" << endl;
        exit(1);
    }

    if (close(fileDescriptor) < 0) {
        cerr << "wcat: error closing file" << endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0; // No files specified, just exit
    }

    for (int i = 1; i < argc; ++i) {
        printFile(argv[i]);
    }

    return 0;
}
