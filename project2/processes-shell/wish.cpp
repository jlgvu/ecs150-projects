#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>

using namespace std;

const int MAX_ARGS = 256;
const int MAX_COMMAND_LENGTH = 1024;

// Function to print the shell prompt
void print_prompt() {
    cout << "wish> ";
}

// Function to print the error message to stderr
void print_error_message() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Function to execute a command with arguments
void execute_command(const char* command, char* const args[], const vector<string>& path_directories) {
    // Try executing the command from each directory in the path
    for (const string& directory : path_directories) {
        string full_command = directory + "/" + command;
        if (access(full_command.c_str(), X_OK) == 0) {
            // Command found and is executable
            pid_t pid = fork();
            if (pid < 0) {
                // Fork failed
                print_error_message();
            } else if (pid == 0) {
                // Child process
                // Execute the command
                execv(full_command.c_str(), args);
                // If execv returns, it indicates an error
                print_error_message();
                exit(EXIT_FAILURE);
            } else {
                // Parent process
                // Wait for the child process to complete
                wait(NULL);
                return; // Exit after the command is executed
            }
        }
    }
    
    // If the command is not found in any directory in the path
    print_error_message();
}

// Function to check if the command is "exit"
bool is_exit_command(const char* command) {
    return strcmp(command, "exit") == 0;
}

int main(int argc, char* argv[]) {
    bool interactive_mode = true;
    FILE* input_file = stdin;
    string batch_filename;
    vector<string> path_directories = {"/bin"}; // Initial path with /bin

    // Check if batch mode is enabled
    if (argc > 1) {
        // Batch mode
        interactive_mode = false;
        batch_filename = argv[1];
        input_file = fopen(batch_filename.c_str(), "r");
        if (!input_file) {
            // Error opening batch file
            print_error_message();
            exit(EXIT_FAILURE);
        }
    }

    // Main loop
    while (true) {
        if (interactive_mode) {
            // Print shell prompt in interactive mode
            print_prompt();
        }

        // Read command from input
        char command_line[MAX_COMMAND_LENGTH];
        if (fgets(command_line, sizeof(command_line), input_file) == NULL) {
            // End of file or error
            if (interactive_mode) {
                // Exit gracefully in interactive mode
                cout << endl;
            }
            break;
        }

        // Parse command line into command and arguments
        // For now, let's assume the command is just one word
        char* command = strtok(command_line, " \n");
        if (command != NULL) {
            // Check if the command is "exit"
            if (is_exit_command(command)) {
                // Exit the shell
                exit(0);
            }

            // Check if the command is "path"
            if (strcmp(command, "path") == 0) {
                // Update path with the arguments passed to "path" command
                path_directories.clear();
                char* directory = strtok(NULL, " \n");
                while (directory != NULL) {
                    path_directories.push_back(directory);
                    directory = strtok(NULL, " \n");
                }
                continue; // Continue to next iteration without executing the command
            }

            // Execute the command
            execute_command(command, NULL, path_directories);
        }
    }

    return 0;
}
