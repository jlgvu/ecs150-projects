#include <iostream>
#include <fcntl.h>
#include <unistd.h>

//Print out content of files
void printFile(const char* filename) {
    //Open file (read only)
    int fileDescriptor = open(filename, O_RDONLY);
    //Check if file actually opened
    if (fileDescriptor < 0) {
        const char* errorMsg = "wcat: cannot open file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }
    // Buffer to read from file
    char buffer[4096]; 
    // Store # bytes read
    ssize_t bytesRead;

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead) {
            //Print error message to standard output
            const char* errorMsg = "wcat: error writing to stdout\n";
            write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
            //exit with status code 1
            exit(1);
        }
    }

    // Check if read operation encountered error
    if (bytesRead < 0) {
        const char* errorMsg = "wcat: error reading from file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    // Close File
    if (close(fileDescriptor) < 0) {
        const char* errorMsg = "wcat: error closing file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    //Check if any files specified
    if (argc < 2) {
        return 0; 
    }

    // For each file in command line argument, call printFile
    for (int i = 1; i < argc; ++i) {
        printFile(argv[i]);
    }

    return 0;
}

