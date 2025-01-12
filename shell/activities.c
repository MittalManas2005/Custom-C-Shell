# include "activities.h"


// data strucuture to store the information about processes
// typedef struct {
//     char command_name[100];  
//     pid_t pid;               
//     char state[10];     
//     int isForeground; 
//     struct timeval start_time;     
// } ProcessInfo;

// contains all the processes spawned by out shell
ProcessInfo process_table[MAX_PROCESSES];
int process_count = 0;

void add_process(char *command_name, pid_t pid, char *state, int isForeground) {
    if(pid == -1)
    return;
    // printf("Entering in list\n");
    if (process_count < MAX_PROCESSES) {
        strcpy(process_table[process_count].command_name, command_name);
        process_table[process_count].pid = pid;
         process_table[process_count].isForeground = isForeground;
        strcpy(process_table[process_count].state, state);
        gettimeofday(&process_table[process_count].start_time, NULL);
        process_count++;
    } else {
        printf("Process table full. Update MAX_PROCESSES in activities.c!\n");
    }
}
// updating a given pid
void update_process_state(pid_t pid, char *new_state) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            strcpy(process_table[i].state, new_state);
            break;
        }
    }
}
void update_isForeground_by_pid(pid_t pid, int new_isForeground) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].isForeground = new_isForeground;
            break;
        }
    }
}
// removing a given pid
void remove_process(pid_t pid) {
    int found = 0;
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            found = 1;
        }
        if (found && i < process_count - 1) {
            process_table[i] = process_table[i + 1];
        }
    }
    if (found) {
        process_count--;
    }
}

int compare_processes(const void *a, const void *b) {
    const ProcessInfo *process_a = (const ProcessInfo *)a;
    const ProcessInfo *process_b = (const ProcessInfo *)b;
    return strcmp(process_a->command_name, process_b->command_name);
}

void display_process_table() {
    // printf("Command Name\tPID\tState\n");
    for(int i = 0 ; i < process_count; i++)
    {
        check_proc_state(process_table[i].pid);
    }
    if(process_count == 0)
    {
        printf("No entry\n");
        return;
    }
    qsort(process_table, process_count, sizeof(ProcessInfo), compare_processes);

    for (int i = 0; i < process_count; i++) {
        printf("%s : %d - %s\n", process_table[i].command_name, process_table[i].pid, process_table[i].state);
    }
}
int activities_handler(char ** strings, int index){
    if(strings[index] == NULL) // nothing ahead{
    {
        display_process_table();
    }
    else printf("Invalid arguments\n");
    return 0;
}
struct timeval get_start_time_by_pid(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            return process_table[i].start_time;
        }
    }
    struct timeval null_time = {0, 0}; // Return a zeroed timeval if not found
    return null_time;
}
double calculate_elapsed_time(struct timeval start_time) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    long seconds = current_time.tv_sec - start_time.tv_sec;
    long microseconds = current_time.tv_usec - start_time.tv_usec;
    return seconds + microseconds / 1e6;
}
int is_pid_present(pid_t pid)
{
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            return 1;
        }
    }
    return 0; 
}
int is_foreground_by_pid(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            return process_table[i].isForeground;
        }
    }
    return -1; // Return -1 if no process with the given PID is found
}

char* get_command_name_by_pid(pid_t pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            return process_table[i].command_name;
        }
    }
    return NULL; // Return NULL if no process with the given PID is found
}
ProcessInfo* get_process_by_pid(pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid) {
            return &process_table[i];
        }
    }
    return NULL; // PID not found
}
void check_proc_state(pid_t pid) {
    if (is_pid_present(pid) == 0) {
        return;
    }

    char proc_status_path[50];
    snprintf(proc_status_path, 50, "/proc/%d/status", pid);

    FILE *file = fopen(proc_status_path, "r");
    if (file == NULL) {
        // printf("Cannot find process %d\n", pid);
        remove_process(pid);  // If the process no longer exists, remove it
        return;
    }

    char line[256];
    char state[64] = "unknown";  // Initialize state to 'unknown'
    
    // Read through the file to find the "State" field
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%63s", state);  // Extract the state value
            break;
        }
    }
    
    fclose(file);

    // Print the extracted state for verification
    // printf("State: %s\n", state);

    // Update the process state based on the extracted state
    if (strcmp(state, "R") == 0) {
        update_process_state(pid, "running");
    } else if (strcmp(state, "T") == 0) {
        update_process_state(pid, "stopped");
    } else if (strcmp(state, "Z") == 0) {
        update_process_state(pid, "running");
    } else if (strcmp(state, "S") == 0 || strcmp(state, "D") == 0) {
        update_process_state(pid, "running");
    } else {
        update_process_state(pid, "unknown");
    }
}
void terminate_all_processes() {
    for (int i = 0; i < process_count; i++) {
        pid_t pid = process_table[i].pid;
        
        // Send SIGTERM to the process
        if (kill(pid, SIGTERM) == 0) {
            // printf("Sent SIGTERM to process %d (%s)\n", pid, process_table[i].command_name);
        } else {
            perror("Failed to send SIGTERM");
        }
    }
}