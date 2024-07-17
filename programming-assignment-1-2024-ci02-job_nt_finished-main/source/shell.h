#ifndef SHELL_H
#define SHELL_H

#include <limits.h> // For PATH_MAX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


#define MAX_LINE 1024
#define MAX_ARGS 64
#define BIN_PATH "./bin/"

// Array of built-in commands

extern const char *builtin_commands[]; 

// Handler function declarations
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_usage(char **args);
int list_env(char **args);
int set_env_var(char **args);
int unset_env_var(char **args);
int print_history(char **args);
int set_theme(char **args);
int shell_ld(char **args);

// // Function declarations for reading commands and displaying the prompt
void read_command(char **cmd);
void type_prompt();

// Helper function to get the number of built-in commands
 int num_builtin_functions();
//     // return sizeof(builtin_commands) / sizeof(char *);


// Function to execute built-in command
int execute_builtin_command(char **cmd);

#endif // SHELL_H