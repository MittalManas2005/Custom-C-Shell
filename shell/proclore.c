# include "proclore.h"
# define MAX_SIZE 4096 

   void printInfo(pid_t pid) {
    // Define the path to the status file
    char path[MAX_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    // Open the file for reading
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Variables to store the extracted details
    char line[MAX_SIZE];
    char status[64] = "Unknown";
    char process_group[64] = "Unknown";
    char vm_size[64] = "Unknown";

    // Read and parse the file contents
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%63s", status);
        } else if (strncmp(line, "Pid:", 4) == 0) {
            sscanf(line, "Pid:\t%d", &pid);
        } else if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "VmSize:\t%63s", vm_size);
        } else if (strncmp(line, "Tgid:", 5) == 0) {
            sscanf(line, "Tgid:\t%d", &pid);
            snprintf(process_group, sizeof(process_group), "%d", pid);
        }
    }

    // Close the file
    fclose(file);
    // deciding whether to put the + or not 
    if(is_foreground_by_pid(pid) == -1)
    {
        pid_t pgid = getpgid(pid);
        pid_t fg_pgid = tcgetpgrp(STDIN_FILENO);

        if (pgid == fg_pgid) {
            // printf("Process is in the foreground\n");
            strcat(status, "+");
        } else {
            // printf("Process is in the background\n");
        }        
    }
    else if (is_foreground_by_pid(pid) == 1)
    {
        strcat(status, "+");
    }
    else
    {
        // do nothing
    }

    // Print the extracted details
    printf("PID: %d\n", pid);
    printf("Process Status: %s\n", status);
    printf("Process Group: %s\n", process_group);
    printf("Virtual Memory: %s\n", vm_size);

    // Get and print the executable path
    char exe_path[MAX_SIZE];
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);
    char real_path[MAX_SIZE];
    ssize_t len = readlink(exe_path, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0'; // Null-terminate the string
        printf("Executable Path: %s\n", real_path);
    } else {
        printf("Error reading executable path\n");
    }
}


void proc_handler(char ** strings ,int index)
{
    pid_t pid;

    if(strings[index] == NULL) // assuming nothign provided after the flags
    {
        pid = getpid();
    }
    else {
        int result = custom_atoi(strings[index]);
        if(result == -1)
        {
            printf("Enter valid pid\n");
            return;
        }
        else pid = result;
    }
    printInfo(pid);
    return;
}