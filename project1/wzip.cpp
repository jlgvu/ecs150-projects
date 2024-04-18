#include <iostream>   
#include <fcntl.h>    
#include <unistd.h>   
#include <cstring>    
#include <vector>     
#include <climits>    

// Compile command: g++ -o wzip wzip.cpp -Wall -Werror

using namespace std;

// Function to write a run of characters to standard output
void writeRun(int count, char character) {
    while (count > 0) {
        // Limit run count to fit within 2 bytes (0xFFFF)
        int runCount = min(count, 0xFFFF);
        // Write the run count as 2-byte binary data to standard output
        write(STDOUT_FILENO, &runCount, sizeof(runCount));
        // Write the character as ASCII to standard output
        write(STDOUT_FILENO, &character, sizeof(character));
        // Decrement the remaining count for the run
        count -= runCount;
    }
}

// Function to compress files using run-length encoding
void wzip(const vector<string>& files) {
    // Check if files vector is empty
    if (files.empty()) {
        // Print usage message to standard error
        cerr << "wzip: file1 [file2 ...]" << endl;
        // Exit with error code 1
        exit(1);
    }

    // Iterate over the files vector
    vector<string>::const_iterator it;
    for (it = files.begin(); it != files.end(); ++it) {
        // Open the current file for reading only
        int fileDescriptor = open(it->c_str(), O_RDONLY);
        // Check if file opening was successful
        if (fileDescriptor < 0) {
            // Print error message to standard error
            cerr << "wzip: cannot open file " << *it << endl;
            // Exit with error code 1
            exit(1);
        }

        // Initialize variables for processing the file
        char prevChar = '\0';
        char currentChar;
        int count = 0;

        // Read characters from the file until end of file
        while (read(fileDescriptor, &currentChar, sizeof(currentChar)) > 0) {
            // If current character is the same as previous, increment count
            if (currentChar == prevChar && count < INT_MAX) {
                count++;
            } else {
                // Write the run to standard output
                writeRun(count, prevChar);
                // Update previous character and reset count
                prevChar = currentChar;
                count = 1;
            }
        }

        // Write the final run to standard output
        writeRun(count, prevChar);

        // Close the file after processing
        close(fileDescriptor);
    }
}

int main(int argc, char *argv[]) {
    // Vector to store filenames from command-line arguments
    vector<string> files;

    // Collect filenames from command-line arguments
    for (int i = 1; i < argc; ++i) {
        files.push_back(argv[i]);
    }

    // Compress files using run-length encoding
    wzip(files);

    // Return 0 to indicate successful execution
    return 0;
}
