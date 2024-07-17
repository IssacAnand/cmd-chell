#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Command to execute ps and filter for dspawn processes
    const char *command = "ps -ef | grep dspawn | grep -Ev 'tty|pts|grep' | wc -l";
    char buffer[128];
    FILE *pipe;

    // Open the command for reading
    pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen failed");
        return EXIT_FAILURE;
    }

    // Read the output of the command
    fgets(buffer, sizeof(buffer), pipe);
    int count = atoi(buffer);

    // Close the pipe
    pclose(pipe);

    // Print the result
    if (count == 0) {
        printf("No daemon is alive right now.\n");
    } else {
        printf("Live daemons: %d\n", count);
    }

    return EXIT_SUCCESS;
}
