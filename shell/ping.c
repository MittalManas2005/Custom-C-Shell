# include "ping.h"

int ping_handler(char ** strings, int index)
{
    if(strings[index] == NULL || strings[index + 1] == NULL)
    {
        printf("Enter complete arguements\n");
        return 0;
    }
    else if(strings[index + 2] != NULL)
    {
        printf("Too much arguments\n");
        return 0;
    }
    else 
    {
        pid_t pid = custom_atoi(strings[index]);
        int signalNumber = custom_atoi(strings[index + 1]) % 32;
        if(is_pid_present(pid) == 0)
        {
            printf("No such process found");
        }
        else 
        {
            if (kill(pid, signalNumber) == 0) {
                printf("Sent signal %d to process with pid %d\n", signalNumber, pid);
            } else {
                perror("Failed to send signal");
            }
        }
    }
    return 0;
}