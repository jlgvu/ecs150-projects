#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <vector>

//g++ -o wzip wzip.cpp -Wall -Werror

using namespace std;

void writeRun(int count, char character) {
    while (count > 0) {
        int runCount = min(count, 0xFFFF); // Limit run count to fit within 2 bytes
        write(STDOUT_FILENO, &runCount, sizeof(runCount));
        write(STDOUT_FILENO, &character, sizeof(character));
        count -= runCount;
    }
}

void wzip(const vector<string>& files) {
    if (files.empty()) {
        cerr << "wzip: file1 [file2 ...]" << endl;
        exit(1);
    }

    vector<string>::const_iterator it;
    for (it = files.begin(); it != files.end(); ++it) {
        int fileDescriptor = open(it->c_str(), O_RDONLY);
        if (fileDescriptor < 0) {
            cerr << "wzip: cannot open file " << *it << endl;
            exit(1);
        }

        char prevChar = '\0';
        char currentChar;
        int count = 0;

        while (read(fileDescriptor, &currentChar, sizeof(currentChar)) > 0) {
            if (currentChar == prevChar && count < INT_MAX) {
                count++;
            } else {
                writeRun(count, prevChar);
                prevChar = currentChar;
                count = 1;
            }
        }

        writeRun(count, prevChar);

        close(fileDescriptor);
    }
}

int main(int argc, char *argv[]) {
    vector<string> files;

    // Collect filenames from command-line arguments
    for (int i = 1; i < argc; ++i) {
        files.push_back(argv[i]);
    }

    // Compress files
    wzip(files);

    return 0;
}
