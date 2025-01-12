
# Project : Custom C Shell


## Project Background

This project was developed as part of the _CS3.301 Operating Systems and Networks at IIIT-Hyderabad_.

The primary goal of this project was to deepen our understanding of processes and commonly used system calls, offering insights into the inner workings of high-level commands. Additionally, it involved exploring the technical details of piping and I/O redirection, further enriching our comprehension of operating system functionalities.

## Project Description
Link to course website :  https://karthikv1392.github.io/cs3301_osn/mini-projects/mp1

## Overview
This project implements a custom shell in C that simulates a command-line interface similar to Unix-like environments. The shell provides various functionalities, including command execution, directory navigation, file listing, and command logging, all while maintaining a user-friendly prompt.


## Features 
### 1. **Dynamic Shell Prompt**
The shell displays a prompt in the format `<Username@SystemName:~>` where:
- `Username` is the current user's name.
- `SystemName` is the name of the system.
- The prompt reflects the current working directory, using `~` for the home directory.

### 2. **Command Input Handling**
- Supports commands separated by `;` or `&`.
- Handles random spaces and tabs in input.
- Commands can be executed sequentially or in the background.

### 3. **Directory Navigation (`hop` Command)**
- Changes the current directory and prints the full path.
- Supports both absolute and relative paths.
- Special flags like `.`, `..`, `~`, and `-` are implemented for ease of navigation.

### 4. **File Listing (`reveal` Command)**
- Lists files and directories in lexicographic order.
- Supports flags `-a` (show hidden files) and `-l` (detailed information).
- Color coding differentiates between file types: 
  - Green for executables
  - White for files
  - Blue for directories

### 5. **Command Logging (`log` Command)**
- Keeps track of the last 15 unique commands executed across sessions.
- Supports commands to purge logs and execute commands from history by index.
- Does not log the `log` command itself.

### 6. **System Command Execution**
- Execute system commands (e.g., `emacs`, `gedit`) in foreground or background.
- **Foreground**: Waits for completion; prints execution time if over 2 seconds.
- **Background**: Commands with `&` run without waiting; displays PID and status upon completion.

### 7. **Process Information (`proclore` Command)**
- Displays info about a process or the shell:
  - PID, Status (R/R+/S/S+/Z), Group, Virtual Memory, Executable Path.

### 8. **File Searching (`seek` Command)**
- Searches for files/directories in a specified directory (or current).
- Returns matching paths; flags:
  - `-d`: Only directories.
  - `-f`: Only files.
  - `-e`: Print or change to single match.
- Format:  seek < flags >  < search > < target_directory >

### 9. **I/O Redirection**
- Supports `>`, `>>`, and `<` for output/input redirection:
  - `>`: Redirects output to a file (overwrites if exists).
  - `>>`: Appends output to a file.
  - `<`: Reads input from a file.
- Displays "No such input file found!" if the input file does not exist.
- Creates the output file with permissions 0644 if it does not exist.

### 10. **Pipes**
- Passes output from one command to the input of another using pipes (`|`).
- Supports multiple pipes; commands run sequentially from left to right.
- Displays "Invalid use of pipe" if improperly used (e.g., no command on either side).

### 11. **Redirection with Pipes**
- Combines I/O redirection and pipes, allowing both functionalities to work together.
- Supports any number of pipes but not multiple inputs/outputs from I/O redirection.

### 12. **Activities (`activities` Command)**
- Lists all currently running processes spawned by the shell in lexicographic order.
- Displays the following information for each process:
  - Command Name
  - PID
  - State (running or stopped)
- Format: [pid] : [command name] - [State]


### 13. **Signals (`ping` Command)**
- Sends a signal to a process using `ping <pid> <signal_number>`.
- Displays "No such process found" if the given PID does not exist.
- The signal number is taken modulo 32 to determine the corresponding signal.

#### Keyboard Signals:
- **Ctrl - C**: Sends SIGINT to interrupt the currently running foreground process.
- **Ctrl - D**: Logs out of the shell after terminating all processes.
- **Ctrl - Z**: Pushes the foreground process to the background, changing its state to stopped.

### 14. **Foreground and Background Management**
- **Foreground (`fg <pid>`)**: Brings a running or stopped background process to the foreground, taking control of the terminal. Displays "No such process found" if the PID does not exist.

- **Background (`bg <pid>`)**: Changes a stopped background process to running (in the background). Displays "No such process found" if the PID does not exist.

### 15. **Neonate (`neonate -n [time_arg]` Command)**
- Prints the Process-ID of the most recently created process every `[time_arg]` seconds.
- The output continues until the user presses the key 'x'.

### 16. **iMan (`iMan <command_name>` Command)**
- Fetches man pages from the internet using sockets from `http://man.he.net/`.
- Outputs the man page for the specified command to the terminal (stdout).
- HTML tags and header are removed while displaying the outptu.

## Usage Instructions
1. **Compiling the Shell**
   To compile the project, use:
   
    ```make``` 
2. **Running the Shell**
Execute the shell with :
 
    ```./shell/shell```
3. **Command Examples** 
Some command examples and execution can be find in the screenshots folder.

## Important assumptions : 
1. The previous directory in start is assigned the directory in which the shell started. Until we change the directory once the previous directory and the current working directory remains the same.

2. Reveal just looks for directories.

3. Hop with multiple arguments keep on running until some wrong directory is encountered.

4. Background processes : 

    1. A background process prints its completion message without waiting for user input if no foreground command is running and if some foreground process is running it waits for its completion.

    2. 4 execution scenarios for background proceeses : 

        1. Background process not requiring any I/O like `sleep 5 &` : Normally execute and after executing the parent (our terminal) will print normal / abnormal execution


        2. Background process which just produce output like `echo hello &` : These command would not produce any output (output is set to dev/null) and will terminate normally and parent prints the message of normal/abnormal execution. However `echo hello > output.txt & ` would send the text to output.txt. So it does work but doesn't print anything if output is to terminal.
        

        3. Background process which are interactive like `vi &` : These commands will start and terminate abnormally.
        

        4. Invalid command : Starts background proces (print pid) then print `Enter valid command` and shows abnormally terminated.

5. In log the user commands are stored as it is , doesn't matter if it is errorneous or correct.

6. Time for foreground process printed is the difference between the time it was started and when it ended.

7. In fg , bg  I have only considered for processes spawned by my shell. However we can do this for any process by reading from proc file rather than process table.

8. `&` cannot come between the pipes. It has to be the last command.

9. Only the command just before `&` is run in background. In case of piping unlike bash we do not run entire command in background.

10. Sending an interative process like `vi` in background and then trying to run it in background using bg causes us to lose access to our terminal.

## Some important flags and function and files used in code : 

1. `flag_display_foreground` and `display.c`:

    Functionality : To determine whether time has to printed or not in display.
    
    `= 0` : means print normal display

    `= 1` : some foreground process displayed the custom time message

    This flag works with the `display()` and ` display_foreground(char * commands, int time)` functions.

2. `initialize.c` : 

    Functionality : To initalize some of the global variables and setup signal handlers and provide some common helper functions required by other files.
   
    1. Contains important global variables like prev_directory and home_directory.
    
    2. Contains `calculateRelativeDirectory()` and `custom_atoi()` as a helper function used in other files

4. `proclore.c` : 

    Functionality : looks for the /proc/pid/stat file and print the information for the information. It uses `printInfo()` function to print the information.

5. `utilities.c` :
    
    It includes helper function for other files. It include `int redirection_handler(char ** strings, int * saved_stdin, int * saved_stdout, int * saved_stderr)` and `void restore_redirection(int saved_stdin, int saved_stdout, int saved_stderr)` which contains the logic for I/O redirection.

6. `color.c` : 
    
    contains function for setting and reseting the display color in terminal 

7. `input.c` : 

    Functionality : contains important function to tokenize the input and identify foreground and background processes.

    1) `get_input(char * str)` : takes input from the user
    2) `split_on_semicolon(char * input)` : split the input based on semicolon
    3) `split_on_ampersand(char * input)` : split the input based on ampersand
    4) `parse_input(char str[], int ampersand)` : remove spaces and tokenise the string and call the appropriate execute function based on background or foreground process.
    
    Input also make sure that `" text here "` and `' text here '` are not modified while tokenizing

    Input also handles `CTRL-D` and call the `terminate_all_processes()` in `activities.c`.

9. `log.c` : 
    
    Functionality : saves the user command
    1. `void enterCommand(char * str)` : To enter command in the logfile
    2. `void log_handler(char ** strings, int i)` : Parse the string to check whether purge has been called or execute.
    3. Other helper function : `void log_read()` , `void log_execute(int index)` , `void log_purge()` , `void read_previous_command(char *previous_command)` etc.

10. `seek.c`
    
    Functionality : execute the seek by look recursively into the directory tree for the target directory
    1. `void seek_handler(char ** strings , int i)` : parses the seek command input string.
    2. `seek(char * path , char * match, char * home, int checkdir)` : actual function which looks for the prefix match and then prints the relative path with respect to the target directory.
    3. `void displayRelative(char *cwd, char *home, char *relative_directory)` : to display the path correctly
    4. `int seek_flag_handler(char ** str, int index)` : for handing the flags
    5. `print_file_content(const char *filename)` : to print file if seek -e gives one file

11. `neonate.c`

    Functionality : 
    The code provides a `neonate_handler` function that prints the most recent process ID at specified intervals and exits when 'x' is pressed. It uses terminal raw mode for non-blocking key detection and manages intervals with usleep.

12. `man.c` 

    Functionality : The code defines an `iMan_handler` function that sends an HTTP request to `man.he.net` to fetch content based on a topic, then processes and prints the HTML content after the </head> tag. It includes functions for stripping HTML tags and handling socket communication.

13. `hop.c` 
    
    Functionality : 
    The `hop_handler` function changes directories based on user input, supporting paths like ~, -, and relative or absolute paths. It uses `chdir()` and `getcwd()` to implement this.

14. `reveal.c` 

    Functionality : The code lists directory contents with options for hidden files and detailed views, using flags to control visibility and format. It handles path resolution and file attribute display, including sorting and formatting output.

15. `execute.c` 

    This code handles both foreground and custom commands. It is called by the `parse_input()` function when no `&` is detected. 
    The `execute()` function checks for pipes and use the following functions : 
    
    1. `int execute_custom_command(char ** strings)` : help executing custom commands

    2.  `int execute_command(char ** strings)` : if there is no pipe then this creates a child and do execvp.

    3. `int execute_command_pipe(char ** strings)` : if there is a pipe command then we create child already in execute, so this function is called to do execvp().

    The command that are spawned as foreground have the terminal id as their process group id.

    `foreground_pid` : This variable stores the current foreground process ID. It helps while sending the CTRL - Z signal.

    `is_foreground_running` : This flag checks if some foreground process is running or not.

16. `fg.c`

    The `fg_handler` function brings a background process to the foreground by sending it a `SIGCONT` signal and setting it as the foreground process group. It includes error handling for signal sending and terminal access adjustments.

17. `background.c` 

    This code handles background processes. It is called by the `parse_input()` function when a `&` is detected. 

    In case of piping we use the `execute_command_pipe()` and `execute_custom_command()` functions to execute the command from `foreground.c`. 
    
    The `executebackground()` function checks for pipes and use the following functions : 
    
    1. `int run_command(char ** strings)` : creates a child and then call execvp. Used if there is no piping.

    2. `int run_last_command(char ** strings)` : if there is a pipe command then we create child already in executebackground(), so this function is called to do execvp().

    The command that are spawned as background are assigned their own pid as pgid.

    For background process if after I/O redirection the input or output is still from terminal , in that case it is set to `/dev/null`.

18. `bg.c` 

    The `bg_handler` function brings a background process to the foreground by sending it a `SIGCONT` signal and setting it as the foreground process group. It includes error handling for signal sending and terminal access adjustments.

19. `signal_handler.c` 

    The signal handlers manage process signals: `SIGINT` and `SIGTSTP` handle foreground process interruptions and stopping, updating process states and handling reassignments. `SIGCHLD` handles child process terminations, logging messages, and updating the process list, while `update_foreground_pid` sets the foreground PID when a new foreground process starts.

20. `ping.c` 

    Used to send signals to processes.

21. `activities.c` 

    It has the data structure  `ProcessInfo` which maintains all the process spawned by our shell. It includes various helper function to operate on this data structure.



## MAXIMUM VALUES : 
1. `MAX_SIZE = 4096` For user input strings and paths
2. `MAX_SEGMENTS = 100` Refers to maximum number of semicolon separated command a user can send at once (99 semicolons) and inside each of these 100 commands they can have 100 ampersand separated segments.
3. `MAX_TOKENS = 100` The maximum number of tokens a command can have after removing semicolons and ampersands.
4. `MAX_LOG = 15` The maximum number of command we can store in the log file.
5. `MAX_ENTRIES = 4096` The maximum number of directories that can be printed by reveal.
6. `MAX_PROCESSES = 100` The maximum number of background process that can run simulatenously
7. `MAX_MESSAGES = 100` The maximum number of pending message the background processes can have if some foreground process is running.
8. `MAX_RESPONSE 65535` The maximum response I am expecting from a man page. We can increase it to accomadate large man pages.


## Resources Used : 
1. https://broman.dev/download/The%20Linux%20Programming%20Interface.pdf
2. https://jameshfisher.com/2017/02/24/what-is-mode_t/
3. https://man7.org/linux/man-pages/index.html
4. ChatGPT : used primarily for debugging . Screenshots attached in the resources folder.
5. https://www.linuxhowtos.org/data/6/client.c
6. https://linux.die.net/man/2/waitpid
7. https://man7.org/linux/man-pages/man7/signal.7.html
## 
