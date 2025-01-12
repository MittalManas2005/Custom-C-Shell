// includes all the helper function common to both foreground and background processes

#include "utilities.h"

//helper function to count the number of pipes present
int count_pipes(char **cmds) { // provide and array of strings
    int pipe_count = 0, i = 0;
    while (cmds[i] != NULL) {
        if (strcmp(cmds[i], "|") == 0) pipe_count++;
        i++;
    }
    return pipe_count; // return an integer
}
void setup_pipes(int pipe_count, int pipes[][2]) {
    for (int i = 0; i < pipe_count; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }
}

int isCustomCommand(char *command) {
    if (strcmp(command, "hop") == 0 ||
        strcmp(command, "reveal") == 0 ||
        strcmp(command, "seek") == 0 ||
        strcmp(command, "proclore") == 0 ||
        strcmp(command, "log") == 0 || 
        strcmp(command, "exit") == 0  ||
        strcmp(command, "activities") == 0  ||
        strcmp(command, "ping") == 0  ||
        strcmp(command, "fg") == 0  ||
        strcmp(command, "bg") == 0  ||
        strcmp(command, "neonate") == 0  ||
        strcmp(command , "iMan") == 0 ) 
    {
        return 1;  // True: the command is a custom command
    }
    return 0;  // False: the command is not a custom command
}


// Call this function only after piping , semicolons and ampersand are removed
// it changes the input and the output stream of the pipe and return appropriate integer
// Also preserver the original stdin and stdout for later restoration.

int redirection_handler(char ** strings, int * saved_stdin, int * saved_stdout, int * saved_stderr)
{
    // return types : 
    // -1 : for no I/O redirection required
    // 0 : some kind or error occurred
    // 1 : input redirection. < encountered
    // 2 : output redirection. (overwrite) > encountered
    // 3 : output redirciton . (append mode) >> encountered
    // 4  : both input and output redirection
    int input_redirected = 0;
    int output_redirected = 0;
    *saved_stdin = dup(STDIN_FILENO); // saving the original stdin and stdout
    *saved_stdout = dup(STDOUT_FILENO);
    *saved_stderr = dup(STDERR_FILENO);
    int i = 0 ; 
    int count = 0;
    
    while (strings[count] != NULL)
    {
        count++;
    }
   
    
    for (i = 1 ; i < count;)
    {
        if(strcmp(strings[i], "<") == 0)
        {   
            // input redireciton required . input to be read from strings[i + 1].
            if (strings[i + 1] == NULL) {
                fprintf(stderr ,"Syntax error: No input file specified\n");
                return 0;
            }
            int fd_in = open(strings[i + 1] , O_RDONLY);
            if(fd_in < 0)
            {
                fprintf(stderr ,"No such input file found\n");
                return 0;
            }
            if(dup2(fd_in, STDIN_FILENO) < 0) // changing the input stream stdin to the file present at strings[i+1]
            {
                fprintf(stderr , "Failed to change input stream\n");
                close(fd_in);
                return 0;
            }
            strings[i] = NULL;
            strings[i + 1] = NULL;
            i++;
            close(fd_in);
        }
        else if(strcmp(strings[i], ">") == 0)
        {
            output_redirected = 1;
            // output redirection required
            if (strings[i + 1] == NULL) {
               fprintf(stderr ,"Syntax error: No output file specified\n");
                return 0;
            }
            int fd_out = open(strings[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd_out < 0)
            {
                fprintf(stderr ,"Cannot find/create output file\n");
                return 0;
            }
            if(dup2(fd_out, STDOUT_FILENO) < 0 ){
                 fprintf(stderr , "Failed to change output stream\n");
                close(fd_out);
                return 0;
            }
            strings[i] = NULL; // removing the redirection symbol
            strings[i + 1] = NULL; // removing the file name as we have already changed the pipe
            i++;
            // printf("HELlo\n");
            close(fd_out);
        }
        else if(strcmp(strings[i], ">>") == 0)
        {
             // output redirection required not overwriting though
             output_redirected = 1;
            int fd = open(strings[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (strings[i + 1] == NULL) 
            {
                fprintf(stderr ,"Syntax error: No output file specified\n");
                return 0;
            }
            if(fd < 0)
            {
                fprintf(stderr ,"Cannot find/create output file\n");
                return 0;
            }
            if(dup2(fd, STDOUT_FILENO) < 0 ){
                 fprintf(stderr , "Failed to change output stream\n");
                close(fd);
                return 0;
            }
            strings[i] = NULL;
            strings[i + 1] = NULL;
            i++;
            close(fd);
        }
        i++; // go to the next array elements
    }

    if (input_redirected && output_redirected) {
        return 4;  // Both input and output redirection handled
    } else if (input_redirected) {
        return 1;  // Input redirection handled
    } else if (output_redirected) {
        return 2;  // Output redirection handled
    }
    return -1;
}

void restore_redirection(int saved_stdin, int saved_stdout, int saved_stderr) {
    if (saved_stdin != -1) {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
    if (saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
    if (saved_stderr != -1) {
        dup2(saved_stderr, STDERR_FILENO);
        close(saved_stderr);
    }
}
