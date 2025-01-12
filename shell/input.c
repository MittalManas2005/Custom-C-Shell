# include "input.h"

#define MAX_SIZE 4096
#define MAX_TOKENS 100
#define MAX_SEGMENTS 100

// this function is only for debugging purposes : don't call in final output
void print_input(char ** strings)
{
    
    int i  = 0;
    while (strings[i] != NULL)
    {
        printf("%s\n", strings[i]);
        i++;
    }
}

void parse_input(char str[], int ampersand) {
    char **strings = malloc(MAX_TOKENS * sizeof(char *));
    if (strings == NULL) {
        fprintf(stderr, "Memory allocation failed for strings\n");
        return;
    }

    int count = 0;
    char *token;
    char *rest = str;

    // remove all the extra spaces but make sure that spaces between the "" are not removed and tokenising the input.
    while (*rest != '\0') 
    {
        while (*rest == ' ' || *rest == '\t' || *rest == '\n') {
            rest++;  // Skip leading spaces, tabs, and new lines
        }

        // if an opening brace occurred wait till the closing
        if (*rest == '"') {
            // Token starts with a quote
            rest++;  // Skip the opening quote
            token = rest;

            // Look for the closing quote
            while (*rest != '"' && *rest != '\0') {
                rest++;
            }

            if (*rest == '"') {
                *rest = '\0';  // Replace closing quote with null terminator
                rest++;        // Move past the closing quote
            }
        }  
        else if (*rest == '\'') {
            // Token starts with a quote
            rest++;  // Skip the opening quote
            token = rest;

            // Look for the closing quote
            while (*rest != '\'' && *rest != '\0') {
                rest++;
            }

            if (*rest == '\'') {
                *rest = '\0';  // Replace closing quote with null terminator
                rest++;        // Move past the closing quote
            }
        }  
        else {
            // Regular token
            token = rest;
            while (*rest != ' ' && *rest != '\t' && *rest != '\n' && *rest != '\0') {
                rest++;
            }
        }

        // Null-terminate the token
        if (*rest != '\0') {
            *rest = '\0';
            rest++;
        }

        // Allocate and store the token
        strings[count] = malloc(strlen(token) + 1);
        if (strings[count] == NULL) {
            fprintf(stderr, "Memory allocation failed for token[%d]\n", count);
            for (int i = 0; i < count; i++) {
                free(strings[i]);
            }
            free(strings);
            return;
        }
        strcpy(strings[count], token);
        count++;
    }

    strings[count] = NULL;
    // print_input(strings);
    if (ampersand == 0) {
        int x = execute(strings, 0);
        if (x == 1) {
            exit(EXIT_SUCCESS);
        } else if (x == -1) {
            // Handle invalid command (signal handler could be involved)
        }
    } else {
        executebackground(strings, 0);
    }

    for (int i = 0; i < count; i++) {
        free(strings[i]);
    }
    free(strings);
}

char getLastNonSpaceChar(const char *str) {
    int len = strlen(str);
    for (int i = len - 1; i >= 0; i--) {
        if (!isspace((unsigned char)str[i])) {
            return str[i];
        }
    }
    return '\0';
}
char *trim_spaces(char *str) {
    if (str == NULL || *str == '\0') {
        return str;
    }
    while (isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == '\0') {
        return str;
    }
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}


void split_on_ampersand(char *input) {

    char *original_input = strdup(input);
    if (original_input == NULL) {
        fprintf(stderr, "Memory allocation failed for original_input\n");
        return;
    }

    // Trim and analyze the original input
    char ch = getLastNonSpaceChar(original_input);
    free(original_input);

    input = trim_spaces(input);
    char **segments = malloc(MAX_SEGMENTS * sizeof(char *));
    int c = 0;
    char *segment = strtok(input, "&");
    if (segments == NULL) {
        fprintf(stderr, "Memory allocation failed for segments\n");
        return;
    }


    
    while (segment != NULL) {
        segment = trim_spaces(segment);
        if (*segment == '|') {
            fprintf(stderr, "Parse error near '|'\n");
            // Free previously allocated memory
            for (int i = 0; i < c; i++) {
                free(segments[i]);
            }
            free(segments);
            return;
        }


        segments[c] = strdup(segment);
        if (segments[c] == NULL) {
            fprintf(stderr, "Memory allocation failed for segment[%d]\n", c);
            // Free previously allocated segments before returning
            for (int i = 0; i < c; i++) {
                free(segments[i]);
            }
            free(segments);
            return;
        }
        c++;
        segment = strtok(NULL, "&");
    }

    if (c == 0) {
        free(segments);
        return;
    }

    for (int i = 0; i < c - 1; i++)
    {
       // printf("Identified a background process : %s\n", segments[i]);
        parse_input(segments[i], 1); // all the inner process are background processes
        free(segments[i]);
    }
    // printf("%s", ch);
    // printf("Parsing input done\n");
    if(ch == '&')
    {
        //printf("Identified a background process : %s\n", segments[c-1]);
        parse_input(segments[c-1], 1); // background
    }
    else 
    {
        parse_input(segments[c -1], 0); // non background
    }
    free(segments[c - 1]); // Free the last segment
    free(segments); 
}

void split_on_semicolon(char *input){
    char **segments = malloc(MAX_SEGMENTS * sizeof(char *));
     if (segments == NULL) {
        // Handle allocation failure
        return;
    }
    int c = 0;
    char *segment = strtok(input, ";");
    while (segment != NULL) {
        segment = trim_spaces(segment);
        if (c >= MAX_SEGMENTS) {
            // Handle overflow case, e.g., log an error or break the loop
            break;
        }

        segments[c] = strdup(segment);
        if (segments[c] == NULL) {
            // Handle strdup failure, e.g., log an error and break the loop
            break;
        }

        c++;
        segment = strtok(NULL, ";");
    }
    // printf("Split semicolon : %d\n", c);
    // print_input(segments);
    for (int i = 0; i < c; i++)
    {
        split_on_ampersand(segments[i]);
        free(segments[i]);
    }
     free(segments);
}

void get_input(char * str)
{   
    if(fgets(str, MAX_SIZE, stdin) != NULL)// takes input even with spaces
    {
        str[strcspn(str, "\n")] = '\0'; // remove the last \n with \0
        if (bufferString != NULL) {
            free(bufferString);
            bufferString = NULL;
        }

        bufferString = (char * ) malloc(sizeof(char) * MAX_SIZE);
        if (bufferString == NULL) {
            perror("Failed to allocate memory for bufferString");
            exit(EXIT_FAILURE);
        }
        strcpy(bufferString, str);
    }
    else {
        if (feof(stdin)) {
            // CTRL-D detected (EOF)
            printf("Detected EOF (CTRL-D)\n");
            terminate_all_processes();
            wait(NULL);
            exit(EXIT_SUCCESS); // exit the program on CTRL-D, or handle it as needed
        }
        else {
           
            perror("Failed to read input");
            exit(EXIT_FAILURE);
        }
    }
}

