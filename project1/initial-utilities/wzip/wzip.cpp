#include <iostream>   // For input/output operations
#include <fcntl.h>    // For file control options
#include <unistd.h>   // For POSIX operating system API
#include <cstring>    // For C-style string manipulation
#include <vector>     // For dynamic arrays
#include <climits>    // For integer type constants
#include <stdint.h> // For int32_t


// Compile command: g++ -o wzip wzip.cpp -Wall -Werror

using namespace std;

// Function to write a run of characters to standard output
void writeRun(int count, char character) {
    while (count > 0) {
        // Convert count to 4-byte integer type
        int32_t runCount = min(count, 0xFFFF);
        // Write the run count as 2-byte binary data to standard output
        write(STDOUT_FILENO, &runCount, sizeof(runCount));
        // Write the character as ASCII to standard output
        write(STDOUT_FILENO, &character, sizeof(character));
        // Decrement the remaining count for the run
        count -= runCount;
        //cout << "WRITE RUN: " << "(" << runCount << "," << character << ")" << endl;

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
        // Print the name of the file being processed
        //cout << "Processing file: " << *it << endl;

        // Open the current file for reading
        int fileDescriptor = open(it->c_str(), O_RDONLY);
        // Check if file opening was successful
        if (fileDescriptor < 0) {
            // Print error message to standard error
            cerr << "wzip: cannot open file " << *it << endl;
            // Exit with error code 1
            exit(1);
        } else {
            // Print message indicating successful file opening
            //cout << "File " << *it << " opened successfully" << endl;
        }

        // Initialize variables for processing the file
        //char prevChar = '\0';
        //int count = 0;
        char prevChar;
        char currentChar;
        int count = 1;

        // Read characters from the file until end of file
        while (read(fileDescriptor, &currentChar, sizeof(currentChar)) > 0) {
            // Print the character read from the file
            //cout << "Read character: " << currentChar << endl;
            //cout << "Previous character: " << prevChar << endl;
            // If current character is the same as previous, increment count
            if (currentChar == prevChar && count < INT_MAX) {
                count++;
            } else {
                // Print count and character values
                //cout << "Count: " << count << ", Character: " << prevChar << endl;
                
                // Write the run to standard output
                writeRun(count, prevChar);
                // Update previous character and reset count
                prevChar = currentChar;
                count = 1;
            }
        }

        // Write the final run to standard output
        writeRun(count, prevChar);

        // Print message indicating completion of processing for the file
        //cout << "Finished processing file: " << *it << endl;

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
