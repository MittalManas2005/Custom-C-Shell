# include "log.h"

# define MAX_LOG 15
// this command should create a file and then push the commands into it

int log_count;
char filepath [MAX_SIZE]; 
int count()
{
    int c = 0;
    char ch;
    FILE *log_file_descriptor = fopen(filepath, "r");
    if (log_file_descriptor == NULL) {
        log_file_descriptor = fopen(filepath, "w");
        if (log_file_descriptor == NULL) {
            perror("Error creating file");
            return -1;
        }
    }
    // function to count the number of commands in the log file 
    for (ch = getc(log_file_descriptor); ch != EOF; ch= getc(log_file_descriptor))
    {
        if (ch == '\n') // Increment count if this character is newline
            c = c + 1;
    }
    return c;

    fclose(log_file_descriptor);
}
void log_initialize()
{
    FILE * log_file_descriptor;
    int ret = 0;
    snprintf(filepath, sizeof(filepath), "%s/shell/logfile.txt", home_directory);
    log_file_descriptor = fopen(filepath, "a+");
    if(log_file_descriptor == NULL)
    {
        perror("Can't open file\n");
    }
    else 
    {
        ret = fclose(log_file_descriptor);
        log_count = count(); 
    }

    if (ret != 0) {
        perror("Error closing file");
    } else {
        //printf("File closed successfully\n");
    }
}

void overwrite(char *str)
{
// Open the file for reading and writing
FILE *log_file = fopen(filepath, "r");
if (log_file == NULL) {
    log_file = fopen(filepath, "w");
    log_count = 0;
        if (log_file == NULL) {
            perror("Error opening file for reading");
            return;
        }  
}

// Allocate memory for lines
char **lines = malloc(MAX_LOG * sizeof(char *));
if (lines == NULL) {
    perror("Memory allocation error");
    fclose(log_file);
    return;
}

int line_size = 4096; 
int line_count = 0;

// Read lines from the file
while (line_count < MAX_LOG && (lines[line_count] = malloc(line_size)) != NULL && fgets(lines[line_count], line_size, log_file) != NULL) {
    line_count++;
}

fclose(log_file); // Close the file for now

// Open the file again for writing
log_file = fopen(filepath, "w");
if (log_file == NULL) {
    perror("Error opening file for writing");
    // Free allocated memory before returning
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    return;
}

// Write back the lines, skipping the first one
for (int i = 1; i < line_count; i++) {
    fputs(lines[i], log_file);
}

// Append the new entry at the end
fputs(str, log_file);

// Free memory
for (int i = 0; i < line_count; i++) {
    free(lines[i]); // Free each line
}
free(lines); // Free the array of lines

fclose(log_file); // Close the file
}

void read_previous_command(char *previous_command) {
FILE *log_file = fopen(filepath, "r");
    if (log_file == NULL) {
    log_file = fopen(filepath, "w");
    log_count = 0;
        if (log_file == NULL) {
                perror("Error opening file");
            return;
        }  
    }

char *line = NULL;
size_t len = 0;
ssize_t read;

// Initialize previous_command to an empty string
previous_command[0] = '\0';

// Skip lines up to (log_count - 1)
for (int i = 0; i < log_count - 1; i++) {
    if ((read = getline(&line, &len, log_file)) == -1) {
        // If fewer lines exist than expected, exit the function
        if (line) free(line);  // Free the memory allocated by getline
        fclose(log_file);
        return;
    }
}

// Read and concatenate lines after the skipped lines
while ((read = getline(&line, &len, log_file)) != -1) {
    // Ensure there's enough space in previous_command to concatenate line
    if (strlen(previous_command) + strlen(line) + 1 < 4096) { // Assuming 4096 is the buffer size
        strcat(previous_command, line);
    } else {
        // If buffer is too small, truncate the result (could also return an error if preferred)
        break;
    }
}

// Remove the trailing newline from previous_command
size_t output_length = strlen(previous_command);
if (output_length > 0 && previous_command[output_length - 1] == '\n') {
    previous_command[output_length - 1] = '\0';
}

// Free the memory allocated by getline
if (line) free(line);

fclose(log_file);
}


void enterCommand(char * str)
{
    char previous_commmand[4096] = {0};
    str[strcspn(str, "\n")] = '\0';
    read_previous_command(previous_commmand); // will give teh alst line 
    if(strcmp(previous_commmand , str) == 0)
    {
        return ; // do not store it
    }
    // printf("%sd%sd", previous_commmand , str);
    int len = strlen(str);
    if (len > 0 && str[len - 1] != '\n') {
        str[len] = '\n';
        str[len + 1] = '\0';
    }
// printf("command to be entered : %s", str);
    FILE * log_file_descriptor;
    log_file_descriptor = fopen(filepath, "a");
    
    if (log_file_descriptor == NULL) {
        log_file_descriptor = fopen(filepath, "w");
        log_count = 0;
        if (log_file_descriptor == NULL) {
            printf("Cannot open file\n");
            return;
        }  
    }

    if(log_count >= MAX_LOG)
    {
        fclose(log_file_descriptor);
        overwrite(str);
    }
    else
    {

        ////printf("Final entry : %s", str);
        if(fprintf(log_file_descriptor, "%s" , str)< 0)
        {
            printf("Cannot write to a fiel\n");
        }
        //printf("wrote\n");
        fflush(log_file_descriptor);
        log_count++;
        fclose(log_file_descriptor);
    }
    
}
void log_purge()
{
    // clear the file
    FILE * log_file_descriptor ;
    log_file_descriptor = fopen(filepath , "w");
    fclose(log_file_descriptor); // just clearing up the file
    log_count = 0;
}
void log_execute(int index)
{
    // opening the file in read mode and not executing the command
    index = index - 1;
        if(index >= log_count || index < 0)
        {
            printf("Can't find command(check index)\n");
            return;
        }
        else 
        {
            FILE * log_file_descriptor ;
            char firstLine[MAX_SIZE];
            log_file_descriptor = fopen(filepath , "r");
            if(log_file_descriptor == NULL)
            {
                log_file_descriptor = fopen(filepath, "w");
                log_count = 0;
                if (log_file_descriptor == NULL) {
                    printf("Cannot open file\n");
                    return;
                }  
            }
            fgets(firstLine, sizeof(firstLine), log_file_descriptor);
            fclose(log_file_descriptor);
            char line[MAX_SIZE];
            
            log_file_descriptor = fopen(filepath , "r");
            
            int c = 0;
            // search in the log_count - index line
            while (fgets(line, sizeof(line), log_file_descriptor)) 
            {
                c++;
                if (c == log_count - index) {
                    //printf("Line\n");
                    break;  // Exit the loop after reading the fifth line
                }
            }
        strcpy(bufferString, line); // change the last commadn otherwise it will updated

        fclose(log_file_descriptor);
        line[strcspn(line, "\n")] = '\0';
        split_on_semicolon(line);
    }
}
    
void log_read()
{
    FILE * log_file_descriptor ;
    log_file_descriptor = fopen(filepath , "r");
    if(log_file_descriptor == NULL)
    {
        log_file_descriptor = fopen(filepath, "w");
        log_count = 0;
        if (log_file_descriptor == NULL) {
            printf("Cannot open file\n");
            return;
        }  
    }
    char ch;
    for (ch = getc(log_file_descriptor); ch != EOF; ch= getc(log_file_descriptor))
    {
        printf("%c", ch);
    }
    fclose(log_file_descriptor);
}

void log_handler(char ** strings, int i)
{
    // if the command has been found out to be log the control will come here
    if(strings[i] == NULL)
    {
        // plain log command 
        log_read();
    }
    else if(strcmp(strings[i] , "execute" ) == 0)
    {
        int result = custom_atoi(strings[i + 1]);
        if(result == -1)
        {
            printf("Enter valid index\n");
            return;
        }
        log_execute(result);
    }
    else if(strings[i + 1] == NULL)
    {
        if(strcmp(strings[i], "purge") == 0)
        {
            log_purge();
            return;
        }
        else 
        {
            printf("Enter valid parameter\n");
        }
    }
    else {
        // printf("%s", strings[i]);
        printf("Invalid parameter\n");
    }
}
