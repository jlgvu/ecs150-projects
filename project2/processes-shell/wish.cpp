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
const string DEFAULT_PATH = "/bin"; // Default path

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
void execute_command(const char* command, vector<string>& args, const vector<string>& path_directories) {
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

                // Check if output redirection is requested
                bool redirect_output = false;
                string output_file;
                for (size_t i = 0; i < args.size(); ++i) {
                    if (args[i] == ">") {
                        if (redirect_output) {
                            // Multiple redirection symbols
                            print_error_message();
                            exit(EXIT_FAILURE);
                        }
                        if (i + 1 < args.size()) {
                            redirect_output = true;
                            output_file = args[i + 1];
                            // Remove the redirection symbols and filename from args
                            args.erase(args.begin() + i, args.begin() + i + 2);
                            --i; // Adjust index after removing elements
                        } else {
                            // Missing filename after ">"
                            print_error_message();
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                // Redirect standard output and error if necessary
                if (redirect_output) {
                    int file_descriptor = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (file_descriptor < 0) {
                        print_error_message();
                        exit(EXIT_FAILURE);
                    }
                    dup2(file_descriptor, STDOUT_FILENO);
                    dup2(file_descriptor, STDERR_FILENO);
                    close(file_descriptor);
                }

                // Build the argument array
                vector<char*> exec_args(args.size() + 1);
                for (size_t i = 0; i < args.size(); ++i) {
                    exec_args[i] = const_cast<char*>(args[i].c_str());
                }
                exec_args[args.size()] = nullptr;

                // Execute the command
                execv(full_command.c_str(), exec_args.data());
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

// Function to handle the cd command
void handle_cd_command(const char* directory) {
    // Change directory using chdir
    if (chdir(directory) != 0) {
        // chdir failed
        print_error_message();
    }
}

// Function to handle the path command
void handle_path_command(const vector<string>& args, vector<string>& path_directories) {
    // Clear the existing path vector
    path_directories.clear();

    // Add directories from args to path_directories vector
    for (size_t i = 1; i < args.size(); ++i) {
        path_directories.push_back(args[i]);
    }
}

// Function to check if the command is "exit"
bool is_exit_command(const char* command) {
    return strcmp(command, "exit") == 0;
}

// Function to check if the command is "cd"
bool is_cd_command(const char* command) {
    return strcmp(command, "cd") == 0;
}

// Function to check if the command is "path"
bool is_path_command(const char* command) {
    return strcmp(command, "path") == 0;
}

int main(int argc, char* argv[]) {
    bool interactive_mode = true;
    FILE* input_file = stdin;
    string batch_filename;
    vector<string> path_directories = {DEFAULT_PATH}; // Initial path with DEFAULT_PATH

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
        string command_str(command_line);
        size_t pos = command_str.find_first_of(" \n");
        string command = command_str.substr(0, pos);
        vector<string> args;
        while (pos != string::npos) {
            size_t next_pos = command_str.find_first_of(" \n", pos + 1);
            args.push_back(command_str.substr(pos + 1, next_pos - pos - 1));
            pos = next_pos;
        }

        // Check if the command is "exit"
        if (is_exit_command(command.c_str())) {
            // Exit the shell
            //exit(0);
            char* arg = strtok(command_line, " \n");
            arg = strtok(NULL, " \n"); // Get the next token after the command
            if (arg != NULL) {
                print_error_message();
            } else {
                // Exit the shell
                exit(0);
    }
        }
        // Check if the command is "cd"
        else if (is_cd_command(command.c_str())) {
            // Handle the cd command
            handle_cd_command(args.size() > 0 ? args[0].c_str() : "");
        }
        // Check if the command is "path"
        else if (is_path_command(command.c_str())) {
            // Handle the path command
            handle_path_command(args, path_directories);
        }
        // Execute other commands
        else {
            // Execute the command
            execute_command(command.c_str(), args, path_directories);
        }
    }

    return 0;
}
