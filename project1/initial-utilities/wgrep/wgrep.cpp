#include <iostream> // Standard input/output operations
#include <fcntl.h>  // File control options
#include <unistd.h> // POSIX operating system API
#include <cstring>  // String manipulation functions

using namespace std;
//g++ -o wgrep wgrep.cpp -Wall -Werror

// Function to search for searchTerm in filename
void grepFile(const char* searchTerm, const char* filename) {
    // Open the file for reading
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0) {
        // Print error message if file cannot be opened
        const char* errorMsg = "wgrep: cannot open file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    // Buffer to read from the file
    char buffer[4096];
    ssize_t bytesRead;

    // Loop to read and search file contents line by line
    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        char* ptr = buffer;
        char* end = buffer + bytesRead;
        while (ptr < end) {
            char* lineEnd = strchr(ptr, '\n');
            if (lineEnd != nullptr) {
                *lineEnd = '\0'; // Null-terminate the line
            }
            // Search for searchTerm in the line
            if (strstr(ptr, searchTerm) != nullptr) {
                cout << ptr << endl; // Print the line if searchTerm is found
            }
            if (lineEnd == nullptr) {
                break; // Reached end of buffer
            }
            ptr = lineEnd + 1;
        }
    }

    // Check for read errors
    if (bytesRead < 0) {
        const char* errorMsg = "wgrep: cannot open file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }

    // Close the file
    if (close(fileDescriptor) < 0) {
        const char* errorMsg = "wgrep: cannot open file\n";
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        exit(1);
    }
}

// Main function to handle command-line arguments
int main(int argc, char *argv[]) {
    // Check if no arguments are provided
    if (argc < 2) {
        // Print usage message and exit with status code 1
        cout << "wgrep: searchterm [file ...]" << endl;
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
   // cout << "NICEEE" << endl;
    return 0;
}
