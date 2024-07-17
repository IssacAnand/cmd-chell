#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 1024

void process_line(char *line) {
    // Remove newline character at the end of the line
    line[strcspn(line, "\n")] = 0;

    // Check if the line starts with "PATH="
    if (strncmp(line, "PATH=", 5) == 0) {
        // Set the PATH environment variable
        setenv("PATH", line + 5, 1);
    } else {
        // Treat the line as a command and execute it
        pid_t pid = fork();
        if (pid == 0) {
            // In child process
            char *args[] = {"/bin/sh", "-c", line, NULL};
            execvp(args[0], args);
            // If execvp fails
            perror("execvp failed");
            _exit(1);
        } else if (pid > 0) {
            // In parent process
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("fork failed");
        }
    }
}

void process_rc_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open .cseshellrc");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        process_line(line);
    }

    fclose(file);
}

int main() {
    const char *rc_file = ".cseshellrc";
    process_rc_file(rc_file);

    // Now continue with the rest of the shell program
    // ...

    return 0;
}

