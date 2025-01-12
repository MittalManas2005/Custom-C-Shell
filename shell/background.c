# include "background.h"

#define MAX_PROCESSES 100
#define MAX_MESSAGES 100

char *message_queue[MAX_MESSAGES];
int message_count = 0;



int run_command(char ** strings)
{
    int original_stdin = -1 ;
    int original_stdout = -1; 
    int original_stderr = -1;
  
    int redirection_status = redirection_handler(strings, &original_stdin, &original_stdout, &original_stderr);
    if (redirection_status == 0) {
        return -1; // Error in redirection handling
    }

    pid_t child = fork();
    if(child  == - 1)
    {
        restore_redirection(original_stdin, original_stdout, original_stderr);
        perror("Cannot run background process\n");
        return 0;
    }
    else if(child > 0) // for parent process
    {
        restore_redirection(original_stdin, original_stdout, original_stderr);
        setpgid(child, child); 
        add_process(strings[0] , child , "running", 0);
        printf("%d\n", child); 
        return child;
    }
    else if(child == 0) // child process
    {
        if (isatty(STDOUT_FILENO)) {
            int dev_null = open("/dev/null", O_RDWR); 
                if (dev_null == -1) {
                perror("Failed to open /dev/null");
                exit(EXIT_FAILURE);
            }   
            dup2(dev_null, STDOUT_FILENO);
            dup2(dev_null, STDERR_FILENO);
            close(dev_null);
        }

        if (isatty(STDIN_FILENO)) {
            int dev_null_stdin = open("/dev/null", O_RDONLY);
            if (dev_null_stdin == -1) {
                perror("Failed to open /dev/null for stdin");
                exit(EXIT_FAILURE);
            }
            dup2(dev_null_stdin, STDIN_FILENO);
            close(dev_null_stdin);
        }

        int arg_count = 0;
        while (strings[arg_count] != NULL) {
            arg_count++;
        }

        char *argument_list[arg_count + 1];
        for (int i = 0; i < arg_count; i++) {
            argument_list[i] = strings[i];
        }

        argument_list[arg_count] = NULL; 
        
        int status_code = execvp(strings[0] , argument_list);
        if(status_code == -1 )
        {
            restore_redirection(original_stdin, original_stdout,original_stderr);
            printf("Enter valid command\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
} 
int run_last_command(char ** strings)
{
    int original_stdin = -1 ;
    int original_stdout = -1; 
    int original_stderr = -1;
   
    int redirection_status = redirection_handler(strings, &original_stdin, &original_stdout, &original_stderr);
    if (redirection_status == 0) {
        return -1; // Error in redirection handling
    }

    if (isatty(STDOUT_FILENO)) {
        int dev_null = open("/dev/null", O_RDWR); 
            if (dev_null == -1) {
            perror("Failed to open /dev/null");
            exit(EXIT_FAILURE);
        }   
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        close(dev_null);
    }

    if (isatty(STDIN_FILENO)) {
        int dev_null_stdin = open("/dev/null", O_RDONLY);
        if (dev_null_stdin == -1) {
            perror("Failed to open /dev/null for stdin");
            exit(EXIT_FAILURE);
        }
        dup2(dev_null_stdin, STDIN_FILENO);
        close(dev_null_stdin);
    }

    int arg_count = 0;
    while (strings[arg_count] != NULL) {
        arg_count++;
    }

    char *argument_list[arg_count + 1];
    for (int i = 0; i < arg_count; i++) {
        argument_list[i] = strings[i];
    }
    argument_list[arg_count] = NULL; 

    int status_code = execvp(strings[0] , argument_list);
    if(status_code == -1 )
    {
        restore_redirection(original_stdin, original_stdout,original_stderr);
        printf("Enter valid command\n");
        exit(EXIT_FAILURE);
    }
    restore_redirection(original_stdin, original_stdout, original_stderr);
    exit(EXIT_SUCCESS);
}


int executebackground(char ** strings, int index) 
{
    int original_stdin = -1 ;
    int original_stdout = -1; 
    int original_stderr = -1;
    if (strcmp(strings[0], "log") != 0)
        enterCommand(bufferString);

    int pipe_count = count_pipes(strings); 
    if (pipe_count == 0) {
        run_command(strings); 
        return 0;
    }
    int pipes[pipe_count][2]; 
    setup_pipes(pipe_count, pipes); 

    int cmd_index = 0; 
    int pid;
    int i = 0;

    // Process all commands except the last one, just process p command not p + 1
    for(int x = 0 ; x < pipe_count ; x++){ // run all the starting commands
        char *cmd[50];  // for an individual piped command
        int j = 0;
        // Extract the command segments between pipes
        while (strings[i] != NULL && strcmp(strings[i], "|") != 0) {
            cmd[j++] = strings[i++];
        }
        cmd[j] = NULL;  

        pid = fork();
        
        if (pid == 0) {  // Child process
            if (cmd_index > 0) {
                // if it is not the first command, take input from previous
                dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
            }
            if (cmd_index < pipe_count) {
                // if it is not the last command, output to the next
                dup2(pipes[cmd_index][1], STDOUT_FILENO);
            }

            // Close all pipe ends in this child
            for (int k = 0; k < pipe_count; k++) {
                close(pipes[k][0]);
                close(pipes[k][1]);
            }

            // Execute the command if it is custom
            if (isCustomCommand(cmd[0]) == 1) {
                execute_custom_command(cmd);
            } else {
                kill(getppid(), SIGUSR2);
                setpgid(pid , tcgetpgrp(STDIN_FILENO)); 
                execute_command_pipe(cmd);
            }
            exit(EXIT_SUCCESS); 
        } else if (pid < 0) {
            perror("fork failed");
            return -1;
        }
        cmd_index++;
        i++; // Move to the next command after the pipe
    }
    
    char *last_cmd[50];
    int j = 0;
    while (strings[i] != NULL) {
        last_cmd[j++] = strings[i++];
    }
    last_cmd[j] = NULL;

    pid = fork();
    
    // input redirection for the child
    if(pid > 0)
    {
        restore_redirection(original_stdin, original_stdout, original_stderr);
        setpgid(pid, pid); 
        add_process(last_cmd[0] , pid , "running", 0);
        printf("%d\n", pid);
    }
    else if (pid == 0) { 
        
        dup2(pipes[cmd_index - 1][0], STDIN_FILENO);
        
        for (int k = 0; k < pipe_count; k++) {
            close(pipes[k][0]);
            close(pipes[k][1]);
        }
        run_last_command(last_cmd);
        exit(EXIT_SUCCESS);
    }

    for (i = 0; i < pipe_count; i++) {
        wait(NULL); // do not wait for the last one
    }
    for (i = 0; i < pipe_count; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    return 0;
}
