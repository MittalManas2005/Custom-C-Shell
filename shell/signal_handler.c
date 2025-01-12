# include "signal_handler.h"


void handle_sigint(int sig) {
    // printf("SIGINT Raised: %d\n", foreground_pid);
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
        waitpid(foreground_pid, NULL, 0);
        foreground_pid = -1;
        remove_process(foreground_pid);
    }
    sigint_received = 1;
    is_foreground_running = 0;
}

void handle_sigtstp(int sig) {
    // printf("\nSIGTSTP Received\n");
    
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
        update_process_state(foreground_pid, "Stopped");
        update_isForeground_by_pid(foreground_pid, 0);
        if (setpgid(foreground_pid, foreground_pid) == -1) {
            // perror("setpgid"); // it will always be denied
        }
        foreground_pid = -1;
    }
    sigtstp_received = 1;
    is_foreground_running = 0;
    signal(SIGTSTP, handle_sigtstp);
}

void handle_sigchld(int signum) 
{
    //  printf("SIGCHLD Raised\n");
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED ) ) > 0) 
    { 
        ProcessInfo *process = get_process_by_pid(pid); // get the process
        if(process)
        {
            char *message = (char *)malloc(100 * sizeof(char));
            if(process->isForeground == 0) // so it is background
            {
                if (WIFEXITED(status) )
                {
                    if (WEXITSTATUS(status) == 0) 
                    {
                        snprintf(message, 100, "%s exited normally (%d)\n", get_command_name_by_pid(pid),pid);
                    } 
                    else {
                        snprintf(message, 100, "%s exited abnormally (%d)\n", get_command_name_by_pid(pid) ,pid);
                    }
                    remove_process(pid);
                }
                else if(WIFSTOPPED(status))
                {
                    update_process_state(pid, "stopped");
                }
                else if (WIFSIGNALED(status)) 
                {
                    remove_process(pid);
                }

                if(is_foreground_running == 0) // no foreground running so print
                    printf("%s", message);
                else  message_queue[message_count++] = message;
            }
            else 
            {
                if (WIFEXITED(status)) {
                    struct timeval start = get_start_time_by_pid(pid);
                    double elapsed = calculate_elapsed_time(start);
                    if(elapsed >= 2) // if it is greater than 2 seconds
                    {
                        display_foreground(get_command_name_by_pid(pid), (int)elapsed);
                        flag_display_foreground = 1;
                    }
                    remove_process(pid);
                    foreground_pid = -1;
                    is_foreground_running = 0;
                }
                else if(WIFSTOPPED(status))
                {
                    update_process_state(pid, "stopped");
                }
                else if (WIFSIGNALED(status)) 
                { // termination by signals
                    int signal_number = WTERMSIG(status);
                    if (signal_number == SIGKILL) {
                        snprintf(message, MAX_MESSAGES, "%s was killed by SIGKILL (%d)\n", get_command_name_by_pid(pid), pid);
                    }
                    else if (signal_number == SIGTERM) 
                    {
                        snprintf(message, 100, "%s was terminated by SIGTERM (%d)\n", get_command_name_by_pid(pid), pid);
                    }
                     else {
                        snprintf(message, MAX_MESSAGES, "%s was terminated by signal %d (%d)\n", get_command_name_by_pid(pid), signal_number, pid);
                    }
                    remove_process(pid);
                }
            }
            check_proc_state(pid); // update the state
        }
    }    
}

void update_foreground_pid (int signum)
{
    foreground_pid = global_foreground_pid; // Set the foreground PID to the current child's PID
    add_process(global_command , global_foreground_pid, "running", 1); // Add the process to the process list
    is_foreground_running = 1; 
}