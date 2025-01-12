# include "bg.h"


int bg_handler(char ** strings, int index)
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
        if (kill(pid, SIGCONT) == -1) {
            perror("Error sending SIGCONT");
            return -1;
        }

        // if (tcsetpgrp(pid, pid) == -1) {
        //     perror("Error setting background process group");
        //     return -1;
        // }
        update_process_state(pid, "running");
    }
    else 
    {
        printf("No such process found");
    }
}