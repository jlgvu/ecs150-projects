#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>

using namespace std;

const int MAX_ARGS = 256;
const int MAX_COMMAND_LENGTH = 1024;

void print_prompt() {
    cout << "wish> ";
}

void print_error_message() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void execute_command(const char* command, char* const args[]) {
    pid_t pid = fork();
    if (pid < 0) {
        print_error_message();
    } else if (pid == 0) {
        // Child process
        if (execvp(command, args) < 0) {
            print_error_message();
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        wait(NULL);
    }
}

int main(int argc, char* argv[]) {
    bool interactive_mode = true;
    FILE* input_file = stdin;
    string batch_filename;

    // Check if batch mode is enabled
    if (argc > 2) {
        print_error_message();
        exit(EXIT_FAILURE);
    } else if (argc == 2) {
        interactive_mode = false;
        batch_filename = argv[1];
        input_file = fopen(batch_filename.c_str(), "r");
        if (!input_file) {
            print_error_message();
            exit(EXIT_FAILURE);
        }
    }

    // Main loop
    while (true) {
        if (interactive_mode) {
            print_prompt();
        }

        // Read command
        char command_line[MAX_COMMAND_LENGTH];
        if (fgets(command_line, sizeof(command_line), input_file) == NULL) {
            // End of file or error
            if (interactive_mode) {
                cout << endl;
            }
            break;
        }

        // Parse command
        // Parse the command_line and split it into command and arguments

        // Execute command
        // Call execute_command with the parsed command and arguments
    }

    return 0;
}
