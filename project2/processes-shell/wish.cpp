#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <algorithm>

using namespace std;

// Function to print the error message to stderr
void print_error_message() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Function to execute a command with arguments
void execute_command(const vector<string>& command_line, const vector<string>& path_directories) {
    // Try executing the command from each directory in the path
    for (const string& directory : path_directories) {
        string full_command = directory + "/" + command_line[0];
        if (access(full_command.c_str(), X_OK) == 0) {
            // Command found and is executable
            pid_t pid = fork();
            if (pid < 0) {
                // Fork failed
                print_error_message();
            } else if (pid == 0) {
                // Child process
                // Execute the command
                // Convert vector<string> to array of C strings
                char* args[command_line.size() + 1];
                for (size_t i = 0; i < command_line.size(); ++i) {
                    args[i] = strdup(command_line[i].c_str());
                }
                args[command_line.size()] = nullptr; // Null-terminate the array
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

// Function to parse the command line
vector<string> parse_command_line(const string& command_line) {
    vector<string> tokens;
    istringstream iss(command_line);
    string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to handle the redirection
void handle_redirection(const vector<string>& command_line) {
    size_t pos = command_line.size();
    for (size_t i = 0; i < command_line.size(); ++i) {
        if (command_line[i] == ">") {
            pos = i;
            break;
        }
    }
    if (pos == command_line.size() || pos == command_line.size() - 1) {
        print_error_message();
        return;
    }

    // Redirect stdout to the specified file
    string filename = command_line[pos + 1];
    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd == -1) {
        print_error_message();
        return;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);

    // Execute the command before '>'
    vector<string> cmd(command_line.begin(), command_line.begin() + pos);
    execute_command(cmd, {"."}); // Use current directory as default path
}

// Function to handle parallel commands
void handle_parallel_commands(const vector<string>& command_line, const vector<string>& path_directories) {
    vector<vector<string>> commands;
    vector<string> current_command;
    for (const string& token : command_line) {
        if (token == "&") {
            commands.push_back(current_command);
            current_command.clear();
        } else {
            current_command.push_back(token);
        }
    }
    if (!current_command.empty()) {
        commands.push_back(current_command);
    }

    vector<pid_t> pids;
    for (const auto& cmd : commands) {
        pid_t pid = fork();
        if (pid == -1) {
            print_error_message();
            return;
        } else if (pid == 0) {
            // Child process
            execute_command(cmd, path_directories);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            pids.push_back(pid);
        }
    }

    // Wait for all child processes to finish
    for (pid_t pid : pids) {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char* argv[]) {
    bool interactive_mode = true;
    ifstream input_file;
    string batch_filename;
    vector<string> path_directories = {"/bin", "/usr/bin"}; // Initial path

    // Check if batch mode is enabled
    if (argc > 1) {
        // Batch mode
        interactive_mode = false;
        batch_filename = argv[1];
        input_file.open(batch_filename);
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
            cout << "wish> ";
        }

        // Read command from input
        string command_line;
        if (interactive_mode) {
            getline(cin, command_line);
        } else {
            if (!getline(input_file, command_line)) {
                // End of file or error
                break;
            }
        }

        // Parse command line into command and arguments
        vector<string> tokens = parse_command_line(command_line);

        if (!tokens.empty()) {
            // Check if the command is "exit"
            if (tokens[0] == "exit") {
                if (tokens.size() == 1) {
                    exit(0);
                } else {
                    print_error_message();
                }
            }
            // Check if the command is "cd"
            else if (tokens[0] == "cd") {
                if (tokens.size() == 2) {
                    if (chdir(tokens[1].c_str()) != 0) {
                        print_error_message();
                    }
                } else {
                    print_error_message();
                }
            }
            // Check if the command is "path"
            else if (tokens[0] == "path") {
                if (tokens.size() > 1) {
                    path_directories.assign(tokens.begin() + 1, tokens.end());
                } else {
                    print_error_message();
                }
            }
            // Check if the command contains '>'
            else if (find(tokens.begin(), tokens.end(), ">") != tokens.end()) {
                handle_redirection(tokens);
            }
            // Check if the command contains '&'
            else if (find(tokens.begin(), tokens.end(), "&") != tokens.end()) {
                handle_parallel_commands(tokens, path_directories);
            }
            // Execute other commands
            else {
                execute_command(tokens, path_directories);
            }
        }
    }

    return 0;
}
