#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#define HISTORY_SIZE 100

// ANSI color escape codes
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define THEME_DEFAULT 0
#define THEME_YELLOW 1
#define THEME_GREEN 2

#define DEFAULT_PROMPT_COLOR ANSI_COLOR_BLUE
#define YELLOW_PROMPT_COLOR ANSI_COLOR_YELLOW
#define GREEN_PROMPT_COLOR ANSI_COLOR_GREEN

int current_theme = THEME_DEFAULT;

// Array of built-in command names
const char *builtin_commands[] = {
    "cd",
    "help",
    "exit",
    "usage",
    "env",
    "setenv",
    "unsetenv",
    "history",
    "settheme",
    "ld"
};

/*
Handler of each shell builtin function
*/
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

// Array of function pointers for built-in commands
int (*builtin_command_func[])(char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit,
    &shell_usage,
    &list_env,
    &set_env_var,
    &unset_env_var,
    &print_history,
    &set_theme,
    &shell_ld
};

// Extra history function
char *command_history[HISTORY_SIZE];
int history_count = 0;

void add_to_history(char *cmd) {
    if (history_count < HISTORY_SIZE) {
        command_history[history_count++] = strdup(cmd);
    } else {
        free(command_history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            command_history[i - 1] = command_history[i];
        }
        command_history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

int print_history(char **args) {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, command_history[i]);
    }
    return 1;
}

int set_theme(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "settheme: expected argument\n");
        return 1;
    } else if (strcmp(args[1], "default") == 0) {
        current_theme = THEME_DEFAULT;
        printf("Theme set to default.\n");
    } else if (strcmp(args[1], "yellow") == 0) {
        current_theme = THEME_YELLOW;
        printf("Theme set to yellow.\n");
    } else if (strcmp(args[1], "green") == 0) {
        current_theme = THEME_GREEN;
        printf("Theme set to green.\n");
    } else {
        fprintf(stderr, "settheme: unknown theme %s\n", args[1]);
    }
    return 1;
}

void get_permissions_string(mode_t mode, char *str) {
    str[0] = (S_ISDIR(mode)) ? 'd' : '-';
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

// Handler for 'ld' command
int shell_ld(char **args) {
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;
    char permissions[11];
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (stat(dir->d_name, &file_stat) == 0) {
                get_permissions_string(file_stat.st_mode, permissions);
                printf("%s %s\n", permissions, dir->d_name);
            } else {
                perror("stat");
            }
        }
        closedir(d);
    } else {
        perror("opendir");
        return 1;
    }
    return 1;
}

// Function to get the prompt color based on the current theme
const char* get_prompt_color() {
    switch (current_theme) {
        case THEME_YELLOW:
            return YELLOW_PROMPT_COLOR;
        case THEME_GREEN:
            return GREEN_PROMPT_COLOR;
        case THEME_DEFAULT:
        default:
            return DEFAULT_PROMPT_COLOR;
    }
}

// Function to process .cseshellrc file
void process_rc_file(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Failed to open .cseshellrc file");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "PATH", 4) == 0) {
            char *path_value = strchr(line, '=');
            if (path_value != NULL) {
                path_value++;
                if (setenv("PATH", path_value, 1) != 0) {
                    perror("Failed to set PATH");
                }
            }
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = {"/bin/sh", "-c", line, NULL};
                execvp(args[0], args);
                perror("execvp failed");
                _exit(EXIT_FAILURE);
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
            } else {
                perror("fork failed");
            }
        }
    }

    fclose(file);
}

// Handler for 'cd' command
int shell_cd(char **args) {
    char cwd[PATH_MAX]; // Buffer to hold the current working directory

    if (args[1] == NULL) {
        fprintf(stderr, "cd: expected argument\n");
        return -1; // Indicate failure
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return -1; // Indicate failure
        } else {
            // Get and print the current working directory
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("Current working directory: %s\n", cwd);

                // Update the PATH environment variable after changing directory
                char *path_env = getenv("PATH");
                if (path_env == NULL) {
                    path_env = "";
                }

                // Calculate the required buffer size for new_path
                size_t new_path_size = strlen(cwd) + strlen("/bin:") + strlen(path_env) + 1;

                // Allocate buffer for new_path
                char *new_path = malloc(new_path_size);
                if (new_path == NULL) {
                    perror("malloc");
                    return -1; // Indicate failure
                }

                // Construct new_path
                snprintf(new_path, new_path_size, "%s/bin:%s", cwd, path_env);

                // Set the new PATH environment variable
                if (setenv("PATH", new_path, 1) != 0) {
                    perror("setenv");
                    free(new_path);
                    return -1; // Indicate failure
                }

                // Free the allocated buffer
                free(new_path);
            } else {
                perror("getcwd");
                return -1; // Indicate failure
            }
        }
    }
    return 1; // Indicate success
}

// Handler for 'help' command
int shell_help(char **args) {
    printf("CSEShell Interface\n");
    printf("Usage: command arguments\n");
    printf("The following commands are implemented within the shell:\n");

    for (int i = 0; i < sizeof(builtin_commands) / sizeof(char *); i++) {
        printf("  %s\n", builtin_commands[i]);
    }
    return 1;
}

// Handler for 'exit' command
int shell_exit(char **args) {
    return 0;  // Return 0 to signal the shell to terminate
}

// Handler for 'usage' command
int shell_usage(char **args) {
    if (args[1] == NULL) {
        printf("Command not given: Type usage <command>.\n");
        return 1; // Indicate command was successful, but shell should continue running
    }

    if (strcmp(args[1], "cd") == 0) {
        printf("Type: cd directory_name to change the current working directory of the shell.\n");
    } else if (strcmp(args[1], "help") == 0) {
        printf("Type: help for supported commands\n");
    } else if (strcmp(args[1], "exit") == 0) {
        printf("Type: exit to terminate the shell gracefully\n");
    } else if (strcmp(args[1], "usage") == 0) {
        printf("Type: usage cd/help/exit\n");
    } else if (strcmp(args[1], "env") == 0) {
        printf("Type: env to list all registered env variables\n");
    } else if (strcmp(args[1], "setenv") == 0) {
        printf("Type: setenv ENV=VALUE to set a new env variable\n");
    } else if (strcmp(args[1], "unsetenv") == 0) {
        printf("Type: unsetenv ENV to remove this env from the list of env variables\n");
    } else if (strcmp(args[1], "clear") == 0) {
        printf("The command you gave: clear, is not part of CSEShell's builtin command\n");
    }

    return 1; // Indicate success
}

// Handler for 'env' command
int list_env(char **args) {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        printf("%s\n", *env);
    }
    return 1;
}

// Handler for 'setenv' command
int set_env_var(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "setenv VAR=VALUE\n");
    } else {
        char *env_var = strtok(args[1], "=");
        char *env_value = strtok(NULL, "=");

        if (env_var == NULL || env_value == NULL) {
            fprintf(stderr, "setenv VAR=VALUE\n");
        } else {
            if (setenv(env_var, env_value, 1) != 0) {
                perror("setenv");
            }
        }
    }
    return 1;
}

// Handler for 'unsetenv' command
int unset_env_var(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "unsetenv VAR\n");
    } else {
        if (unsetenv(args[1]) != 0) {
            perror("unsetenv");
        }
    }
    return 1;
}

int execute_builtin_command(char **cmd) {
    for (int i = 0; i < sizeof(builtin_commands) / sizeof(char *); i++) {
        if (strcmp(cmd[0], builtin_commands[i]) == 0) {
            return (*builtin_command_func[i])(cmd);
        }
    }
    return -1; // Command not found
}

// Function to read a command from the user input
void read_command(char **cmd) {
    char line[MAX_LINE];
    int count = 0, i = 0;
    char *array[MAX_ARGS], *command_token;

    for (;;) {
        int current_char = fgetc(stdin);
        line[count++] = (char)current_char;
        if (current_char == '\n')
            break;
        if (count >= MAX_LINE) {
            printf("Command is too long, unable to process\n");
            exit(1);
        }
    }
    line[count] = '\0';

    if (count == 1)
        return;

    if (count > 1) {
        add_to_history(line);
    }

    command_token = strtok(line, " \n");
    while (command_token != NULL) {
        array[i++] = strdup(command_token);
        command_token = strtok(NULL, " \n");
    }

    for (int j = 0; j < i; j++) {
        cmd[j] = array[j];
    }
    cmd[i] = NULL;
}

// Function to display the shell prompt
void type_prompt() {
    static int first_time = 1;
    if (first_time) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        first_time = 0;
    }
    fflush(stdout);
    printf("%s☆☆ " ANSI_COLOR_RESET, get_prompt_color());
}

// The main function where the shell's execution begins
int main(void) {
    char *cmd[MAX_ARGS];
    int child_status, status;
    pid_t pid;

    process_rc_file(".cseshellrc");

    while (1) {
        type_prompt();
        read_command(cmd);

        if (cmd[0] == NULL)
            continue;

        if (execute_builtin_command(cmd) == 0)
            break;

        pid = fork();

        if (pid < 0) {
            printf("Failed to fork the process\n");
            continue;
        } else if (pid == 0) {
            char full_path[PATH_MAX];
            char cwd[1024];

            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                snprintf(full_path, sizeof(full_path), "%s/bin/%s", cwd, cmd[0]);
            } else {
                printf("Failed to get current working directory.");
                exit(1);
            }

            execvp(full_path, cmd);
            exit(1);
        } else {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                child_status = WEXITSTATUS(status);
            }
        }

        for (int i = 0; cmd[i] != NULL; i++) {
            free(cmd[i]);
        }
        memset(cmd, '\0', sizeof(cmd));
    }

    return 0;
}
