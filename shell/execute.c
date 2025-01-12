# include "execute.h"

int flag_display_foreground;


pid_t foreground_pid = -1; 



int execute_command(char ** strings)
{
    int saved_stdin = -1;
    int saved_stdout = -1; 
    int saved_stderr = -1;
    int redirection_status = redirection_handler(strings, &saved_stdin, &saved_stdout, &saved_stderr);
    if (redirection_status == 0) {
        restore_redirection(saved_stdin, saved_stdout, saved_stderr);
        return -1; // Error in redirection handling
    }   

 
    pid_t child = fork();    
    if(child  == - 1)
    {
        perror("Cannot create child process\n");
        return -1;
    }
    else if(child > 0) // for parent process
    {
        restore_redirection(saved_stdin, saved_stdout, saved_stderr);
        int status;
        is_foreground_running = 1; 
        foreground_pid = child; 
        add_process(strings[0], child, "running", 1); 
        while (is_foreground_running) {
            usleep(10000); // Sleep for 10 milliseconds
        }
    }
    else if(child == 0) // child process
    {
        setpgid(child, tcgetpgrp(STDIN_FILENO));
        int arg_count = 0;
        while (strings[arg_count] != NULL) {
            arg_count++;
        }

        char *argument_list[arg_count + 1];
        for (int i = 0; i < arg_count; i++) {
            argument_list[i] = strings[i];
        }

        argument_list[arg_count] = NULL; // NULL-terminate the argument list

        int status_code = execvp(strings[0] , argument_list);
        
        if (status_code == -1) {

            fprintf(stderr, "ERROR : '%s' is not a valid command\n", strings[0]);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

int execute_command_pipe(char ** strings)
{
    
    int saved_stdin = -1;
    int saved_stdout = -1; 
    int saved_stderr = -1;
    int redirection_status = redirection_handler(strings, &saved_stdin, &saved_stdout, &saved_stderr);
    if (redirection_status == 0) {
        restore_redirection(saved_stdin, saved_stdout, saved_stderr);
        return -1; // Error in redirection handling
    }

    int arg_count = 0;
    
    while (strings[arg_count] != NULL) {
        arg_count++;
    }
    
    char *argument_list[arg_count + 1];
    for (int i = 0; i < arg_count; i++) {
        argument_list[i] = strings[i];
    }
    
    argument_list[arg_count] = NULL; // NULL-terminate the argument list
   
    int status_code = execvp(strings[0] , argument_list);
     kill(getppid(), SIGUSR2); // parent will
    if (status_code == -1) {
        
        fprintf(stderr, "ERROR : '%s' is not a valid command\n", strings[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int execute_custom_command(char ** strings)
{
    int saved_stdin = -1;
    int saved_stdout = -1;
    int saved_stderr = -1;
    int redirection_status = redirection_handler(strings, &saved_stdin, &saved_stdout, &saved_stderr);
    if (redirection_status == 0) {
        restore_redirection(saved_stdin, saved_stdout, saved_stderr);
        return -1; // Error in redirection handling
    }

    int i =0;
    if(strcmp(strings[i] , "hop") == 0 )
    {
        hop_handler(strings, i + 1);
    }
    else if(strcmp(strings[i] , "reveal") == 0)
    {
        reveal_handler(strings, i + 1);
    } 
    else if(strcmp(strings[i], "seek") == 0)
    {
        seek_handler(strings , i + 1);
    } 
    else if (strcmp(strings[i], "proclore") == 0 )
    {
        proc_handler(strings, i + 1);
    }
    else if( strcmp(strings[i], "log") == 0)  
    {
        log_handler(strings , i + 1);
    }
    else if(strcmp(strings[i] , "iMan") == 0 )
    {
        iMan_handler(strings, i + 1);
    } 
    else if(strcmp(strings[i] , "activities") == 0 )
    {
        activities_handler(strings, i + 1);
    }
    else if(strcmp(strings[i] , "ping") == 0 )
    {
        ping_handler(strings, i + 1);
    } 
    else if(strcmp(strings[i] , "neonate") == 0 )
    {
        neonate_handler(strings, i + 1);
    } 
    else if(strcmp(strings[i] , "fg") == 0 )
    {
        fg_handler(strings, i + 1);
    } 
    else if(strcmp(strings[i] , "bg") == 0 )
    {
        bg_handler(strings, i + 1);
    } 
    else if(strcmp(strings[i], "exit") == 0 )
    {
        printf("Shell exited successfully\n");
        exit(EXIT_SUCCESS);
        return 1; // special case to signify that exit command as sent
    }
    restore_redirection(saved_stdin, saved_stdout, saved_stderr);
    return 0;
}


// assuming that we can have pipes as well
// Aim of this function is to distinguish between custom and system commands and call appropriate function after piping
// execute function : stores command in log and handlers piping
int execute(char ** strings, int index)
{
    // if the command does not start with log store it in the log file
    if(strcmp(strings[0], "log") !=  0)
        enterCommand(bufferString);
    
    // count for the number of pipes that we have
    int pipe_count = count_pipes(strings);
    if(pipe_count == 0)
    {
        if(isCustomCommand(strings[0]) == 1){
            execute_custom_command(strings); // Execute custom command
        } else {
            execute_command(strings); // Execute standard command
        }
        return 0;
    }
    // create pipes : if pipe_count = 0 then no pipe will be created
    int pipes[pipe_count][2]; 
    setup_pipes(pipe_count, pipes); 
    int cmd_index = 0; 
    int pid;
    // for p pipes we have p + 1 commands
    int i = 0;
    
    for(int x = 0 ; x <= pipe_count; x++){
        // extract out individual command
        char *cmd[50];  
        int j = 0;
        while (strings[i] != NULL && strcmp(strings[i], "|") != 0) {
           
            cmd[j++] = strings[i++]; // extracting individual command separated by pipes
        }
        cmd[j] = NULL;  

        pid = fork();
        if (pid == 0) {  // Child process

            if (cmd_index > 0) {
                dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
            }
            if (cmd_index < pipe_count) {
                dup2(pipes[cmd_index][1], STDOUT_FILENO);
            }

            for (int k = 0; k < pipe_count; k++) {
                close(pipes[k][0]);
                close(pipes[k][1]);
            }

            if(isCustomCommand(cmd[0]) == 1){
                execute_custom_command(cmd); // Execute custom command
            } else {
                setpgid(pid , tcgetpgrp(STDIN_FILENO));
                global_foreground_pid = pid; // Set PID
                global_command = strdup(cmd[0]);  // Make a copy of the command string to avoid issues
                execute_command_pipe(cmd); // Execute standard command
            }
            exit(EXIT_SUCCESS); 
        }
        else if (pid < 0) {
            perror("fork failed");
            return -1;
        }
        cmd_index++;
        i++; // for skipping the pipe symbol
    }
    for (int i = 0; i < pipe_count; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    for (int i = 0; i <= pipe_count; i++) {
        wait(NULL);
    }
    return 0;
}
