#include "system_program.h" // Include your custom header file if needed
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>  

char output_file_path[PATH_MAX]; 

// Daemon work function
static int daemon_work() {
    int num = 0;
    FILE *fptr;
    char *cwd;
    char buffer[1024];

    // write PID of daemon in the beginning
    fptr = fopen(output_file_path, "a");
    if (fptr == NULL) {
        perror("fopen() error");
        return EXIT_FAILURE;
    }

    fprintf(fptr, "Daemon process running with PID: %d, PPID: %d, opening logfile with FD %d\n", getpid(), getppid(), fileno(fptr));

    // then write cwd
    cwd = getcwd(buffer, sizeof(buffer));
    if (cwd == NULL) {
        perror("getcwd() error");
        fclose(fptr);
        return EXIT_FAILURE;
    }

    fprintf(fptr, "Current working directory: %s\n", cwd);
    fclose(fptr);

    while (num < 10) {
        fptr = fopen(output_file_path, "a");
        if (fptr == NULL) {
            perror("fopen() error");
            return EXIT_FAILURE;
        }

        fprintf(fptr, "PID %d Daemon writing line %d to the file.\n", getpid(), num);
        num++;

        fclose(fptr);

        sleep(10);
    }

    return EXIT_SUCCESS;
}

// Function to daemonize the process
void daemonize() {
    pid_t pid, sid;

    // Fork off the parent process (dspawn)
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // Parent process terminates
    if (pid > 0) {
        printf("Daemon spawned successfully.\n");
        exit(EXIT_SUCCESS); 
    }

    // Child process (intermediate) becomes session leader
    sid = setsid();
    if (sid < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // Ignore signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // Fork again to create the actual daemon
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // Intermediate process terminates
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Daemon process:
    umask(0); // Set file permissions

    chdir("/");

    // Close all open file descriptors
    int max_fd = sysconf(_SC_OPEN_MAX);
    for (int fd = 0; fd < max_fd; fd++) {
        close(fd);
    }

    // Redirect standard file descriptors to /dev/null
    open("/dev/null", O_RDWR); // stdin
    dup(0);                    // stdout
    dup(0);                    // stderr
}

int execute(char **args) {
    // Construct log file path in current directory
    if (getcwd(output_file_path, sizeof(output_file_path)) == NULL) {
        perror("getcwd() error");
        return EXIT_FAILURE;
    }
    strncat(output_file_path, "/dspawn.log", sizeof(output_file_path) - strlen(output_file_path) - 1);

    daemonize(); // Daemonize the process before starting daemon_work
    return daemon_work();
}

int main(int argc, char **args) {
    return execute(args);
}