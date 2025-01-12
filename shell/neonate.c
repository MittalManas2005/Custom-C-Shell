# include "neonate.h"

pid_t get_most_recent_pid() {
    DIR *dir;
    struct dirent *entry;
    pid_t recent_pid = 0;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            char *endptr;
            pid_t pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0') {
                if (pid > recent_pid) {
                    recent_pid = pid;
                }
            }
        }
    }

    closedir(dir);
    return recent_pid;
}

// Set terminal to raw mode
void enable_raw_mode(struct termios *orig_termios) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig_termios); // Get original terminal settings
    raw = *orig_termios;

    // Modify terminal settings for raw mode
    raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    raw.c_cc[VMIN] = 1;              // Minimum of 1 character before returning
    raw.c_cc[VTIME] = 0;             // No timeout, read returns immediately

    tcsetattr(STDIN_FILENO, TCSANOW, &raw); // Set terminal to raw mode
}

// Restore terminal to cooked mode
void disable_raw_mode(struct termios *orig_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, orig_termios); // Restore original settings
}

// Check if a key is pressed
int kbhit() {
    struct timeval tv = {0, 0}; // Non-blocking
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}
int neonate_handler(char **strings, int index) {
    if (strings[index] == NULL || strings[index + 1] == NULL) {
        printf("Enter a valid argument\n");
        return 0;
    }
    if (strcmp(strings[index], "-n") != 0) {
        printf("Invalid flag\n");
        return 0;
    }

    index++;
    int interval = custom_atoi(strings[index]);
    if (interval < 0) {
        printf("Invalid interval\n");
        return 0;
    }

    struct termios orig_termios;
    enable_raw_mode(&orig_termios); // Enable raw mode

    while (1) {
        pid_t recent_pid = get_most_recent_pid();
        printf("%d\n", recent_pid);
        fflush(stdout);

        if (interval > 0) {
            // Non-blocking sleep mechanism when interval is greater than 0
            for (int i = 0; i < interval * 1000; i += 100) {
                usleep(100000); // Sleep for 100ms (0.1s)
                if (kbhit()) {   // If a key is pressed
                    char ch = getchar();
                    if (ch == 'x') {
                        disable_raw_mode(&orig_termios); // Restore terminal settings
                        return 0; // Exit on 'x'
                    }
                }
            }
        } else {
            // Continuous printing with no delay for interval 0
            if (kbhit()) {   // Check if a key is pressed without delay
                char ch = getchar();
                if (ch == 'x') {
                    disable_raw_mode(&orig_termios); // Restore terminal settings
                    return 0; // Exit on 'x'
                }
            }
            // No sleep when interval is 0
        }
    }

    disable_raw_mode(&orig_termios); // Restore terminal settings before exiting
    return 0;
}
