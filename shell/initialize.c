
# define MAX_SIZE 4096
# include "initialize.h"

uid_t userID; // contains the user ID
char * userName; // contains the username
char systemName [MAX_SIZE]; // contains the system name
char current_working_directory[MAX_SIZE]; // will always store the complete absolute path for the current_working_directory
char home_directory[MAX_SIZE]; // contains the home directory of the user that is the ~ directory 
char relative_directory[MAX_SIZE]= "~"; // initially the relative directory
char prev_directory[MAX_SIZE] = "~"; // initially the previous directroy
char * bufferString = NULL;
char is_foreground_running = 0;
volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigtstp_received = 0;
pid_t global_foreground_pid = -1;
char *global_command = "\0";

void calculateRelativeDirectory(char *cwd, char *home, char *relative_directory) {
    size_t home_len = strlen(home);
    size_t cwd_len = strlen(cwd);

    if (strncmp(cwd, home, home_len) == 0) {
        relative_directory[0] = '~';
        strcpy(relative_directory + 1, cwd + home_len);
    } else {
        strcpy(relative_directory, cwd);
    }
}
void setup_signals()
{
    //SIGCHLD
    struct sigaction sa_chld;
    memset(&sa_chld, 0, sizeof(sa_chld));
    sa_chld.sa_flags = SA_RESTART | SA_SIGINFO;
    sa_chld.sa_sigaction = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("sigaction(SIGCHLD)");
        exit(EXIT_FAILURE);
    }
    //SIGINT
    struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction(SIGINT)");
        exit(EXIT_FAILURE);
    }
 
    //SIGTSTP    
    struct sigaction sa_tstp;
    memset(&sa_tstp, 0, sizeof(sa_tstp));
    sa_tstp.sa_handler = handle_sigtstp;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = SA_RESTART;
    if (sigaction(SIGTSTP, &sa_tstp, NULL) == -1) {
        perror("sigaction(SIGTSTP)");
        exit(EXIT_FAILURE);
    }
    signal(SIGUSR2, update_foreground_pid);
}
void initialize()
{
    setup_signals();
    message_count = 0;
    userID = getuid();  
    struct passwd * user = getpwuid(userID);  
    userName = user->pw_name;
    gethostname(systemName, 50);
    getcwd(current_working_directory,MAX_SIZE); // entire path
    getcwd(prev_directory, MAX_SIZE);
    strncpy(home_directory, current_working_directory, MAX_SIZE); // saved for future purposes
}
int digit(char c) 
{
    return c >= '0' && c <= '9';
}

int custom_atoi(const char *str) {
    // Check for empty string
    if (*str == '\0') {
        return -1; // Indicate failure (empty string)
    }

    int value = 0;
    while (*str) {
        if (!digit(*str)) {
            return -1; // Indicate failure (non-digit character)
        }
        
        // Check for overflow
        if (value > (INT_MAX - (*str - '0')) / 10) {
            return -1; // Indicate failure (overflow)
        }
        
        value = value * 10 + (*str - '0');
        str++;
    }
   return value;
}
