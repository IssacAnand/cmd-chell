#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main() {
    char *backup_dir = getenv("BACKUP_DIR");
    if (backup_dir == NULL) {
        fprintf(stderr, "Error: BACKUP_DIR environment variable not set.\n");
        return EXIT_FAILURE;
    }

    char zip_filename[256];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Create the zip filename with the current datetime
    strftime(zip_filename, sizeof(zip_filename) - 1, "backup_%Y%m%d%H%M%S.zip", t);

    // Create the zip command
    char zip_command[512];
    snprintf(zip_command, sizeof(zip_command), "zip -r %s %s", zip_filename, backup_dir);

    // Execute the zip command
    if (system(zip_command) == -1) {
        perror("zip command failed");
        return EXIT_FAILURE;
    }

    // Create the move command to move the zip file to the archive directory
    char move_command[512];
    snprintf(move_command, sizeof(move_command), "mv %s ./archive/", zip_filename);

    // Execute the move command
    if (system(move_command) == -1) {
        perror("move command failed");
        return EXIT_FAILURE;
    }

    printf("Backup completed successfully.\n");
    return EXIT_SUCCESS;
}
