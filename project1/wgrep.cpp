#include <iostream> // Standard input/output operations
#include <fcntl.h>  // File control options
#include <unistd.h> // POSIX operating system API
#include <cstring>  // String manipulation functions

//g++ -o wgrep wgrep.cpp -Wall -Werror

using namespace std;
// Function to search for searchTerm in filename or standard input
void grepFile(const char* searchTerm, const char* filename) {
    // File descriptor for standard input
    int stdinDescriptor = STDIN_FILENO;

    // If filename is not standard input, open the file for reading
    if (filename != nullptr && strcmp(filename, "-") != 0) {
        stdinDescriptor = open(filename, O_RDONLY);
        if (stdinDescriptor < 0) {
            // Print error message if file cannot be opened
            const char* errorMsg = "wgrep: cannot open file\n"; 
            write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
            exit(1);
        }
    }

    // Buffer to read from the file
    char buffer[4096];
    ssize_t bytesRead;

    // Loop to read and search file contents line by line
    while ((bytesRead = read(stdinDescriptor, buffer, sizeof(buffer))) > 0) {
        char* ptr = buffer;
        char* end = buffer + bytesRead;
        while (ptr < end) {
            char* lineEnd = strchr(ptr, '\n');
            if (lineEnd != nullptr) {
                *lineEnd = '\0'; // Null-terminate the line
            }
            // Search for searchTerm in the line
            if (strstr(ptr, searchTerm) != nullptr) {
                // Write the line to standard output
                write(STDOUT_FILENO, ptr, strlen(ptr));
                write(STDOUT_FILENO, "\n", 1); // Write newline character
            }
            if (lineEnd == nullptr) {
                break; // Reached end of buffer
            }
            ptr = lineEnd + 1;
        }
    }

    // Check for read errors
    if (bytesRead < 0) {
        const char* errorMsg = "wgrep: cannot read file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    // Close the file if it's not standard input
    if (filename != nullptr && strcmp(filename, "-") != 0) {
        if (close(stdinDescriptor) < 0) {
            const char* errorMsg = "wgrep: cannot close file\n";
            write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
            exit(1);
        }
    }
}


// Main function to handle command-line arguments
int main(int argc, char *argv[]) {
    // Check if no arguments are provided
    if (argc < 2) {
        // Print usage message and exit with status code 1
        const char* errorMsg = "wgrep: searchterm [file ...]\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        return 1;
    }

    // Extract the search term
    const char* searchTerm = argv[1];

    // Check if a single search term is provided without files
    if (argc == 2) {
        // Search for the term in standard input
        grepFile(searchTerm, STDIN_FILENO);
    } else {
        // Loop through each file specified in the command-line arguments
        for (int i = 2; i < argc; ++i) {
            // Search for the term in each file
            grepFile(searchTerm, argv[i]);
        }
    }

    // Exit with status code 0
    return 0;
}

//grep standard input normally: .

