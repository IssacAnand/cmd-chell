#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <pwd.h>

void print_system_info() {
    struct utsname unameData;
    struct passwd *pw;
    uid_t uid;
    char hostname[1024];
    int mib[2];
    size_t len;
    int64_t memsize;
    long uptime;

    // Get OS, kernel, and hostname information
    uname(&unameData);
    gethostname(hostname, sizeof(hostname));

    // Get uptime information
    mib[0] = CTL_KERN;
    mib[1] = KERN_BOOTTIME;
    len = sizeof(uptime);
    sysctl(mib, 2, &uptime, &len, NULL, 0);

    // Get total memory size
    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    len = sizeof(memsize);
    sysctl(mib, 2, &memsize, &len, NULL, 0);

    // Get current user information
    uid = geteuid();
    pw = getpwuid(uid);

    printf("Simple System Information\n");
    printf("OS: %s\n", unameData.sysname);
    printf("Hostname: %s\n", hostname);
    printf("Kernel: %s\n", unameData.release);
    printf("Uptime: %ld seconds\n", uptime);
    printf("User: %s\n", pw->pw_name);
    printf("CPU: %s\n", unameData.machine);
    printf("Memory: %lld MB\n", memsize / 1024 / 1024);
}

int main() {
    print_system_info();
    return 0;
}
