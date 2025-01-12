# include "fg.h"

int fg_handler(char ** strings, int index)
{
    if(strings[index] == NULL)
    {
        printf("Enter complete arugments\n");
        return 0;
    }
    if(strings[index + 1] != NULL)
    {
        printf("Too many arugments\n");
        return 0;
    }
    pid_t pid = custom_atoi(strings[index]);
    if(is_pid_present(pid))
    {
        
        // printf("Sending continue signal\n");
       if (kill(pid, SIGCONT) == -1) {
            perror("Error sending SIGCONT");
            return -1;
        }
        is_foreground_running = 1; 
        foreground_pid = pid; // new thing brought to foreground
        // if (tcsetpgrp(STDIN_FILENO, getpgid(pid)) == -1) {
        //     perror("Error setting foreground process group");
        //     return -1;
        // }
        update_process_state(pid, "running");
        update_isForeground_by_pid(pid , 1);
        while (is_foreground_running) {
            usleep(10000); // Sleep for 10 milliseconds
        }
        
    }
    else 
    {
        printf("No such process found");
    }
}