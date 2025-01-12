# include "seek.h"

// in order to implement this we will use the concept of the reveal commadn of opening a directory and then looking into it

//Stack implementation to convert recursion into stack based implementation

int d_flag = 0; 
int e_flag = 0; 
int f_flag = 0; 
int other_flag = 0;

void print_file_content(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Seek to the end of the file to determine its size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory to store the file content
    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return;
    }

    // Read the file into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0'; // Null-terminate the buffer

    // Print the file content
    printf("%s", buffer);

    // Clean up
    free(buffer);
    fclose(file);
}

int seek_flag_handler(char ** str, int index)
{
    int i = index;
    while (str[i] != NULL)
    {
        if(str[i][0] == '-')
        {
            // so we have encountered a flag
            for (int j = 1; j < strlen(str[i]); j++)
            {
                if(str[i][j] == 'd')
                    d_flag = 1;
                else if(str[i][j] =='f')
                    f_flag = 2;
                else if(str[i][j] == 'e')
                    e_flag = 1;
                else other_flag = 1;
            }
            i++;
        }
        else break;
    }
    return i;
}

typedef struct Node {
    char *path; // linked list kind of node for storing the entiries
    struct Node *next;
} Node;

void push(Node **stack, const char *path) {
    Node *new_node = malloc(sizeof(Node)); // pushing to the node 
    new_node->path = strdup(path);
    new_node->next = *stack;
    *stack = new_node;
}

char *pop(Node **stack) {
    if (*stack == NULL) { // popping out of the node
        return NULL;
    }
    Node *temp = *stack;
    char *path = temp->path;
    *stack = temp->next;
   // free(temp);
    return path;
}
void displayRelative(char *cwd, char *home, char *relative_directory) {
    size_t home_len = strlen(home);
    size_t cwd_len = strlen(cwd);

    if (strncmp(cwd, home, home_len) == 0) {
        relative_directory[0] = '.';
        strcpy(relative_directory + 1, cwd + home_len);
    } else {
        strcpy(relative_directory, cwd);
    }
}
void seek(char * path , char * match, char * home, int checkdir) // assuming we are provided with the path
{
    // it start searching from the path u provide and display with the relative home you had
    struct dirent *entry;
    struct stat fileStat;
    char temp[MAX_SIZE];
    Node * stack = NULL;
    push(&stack, path); // initial directory from where we are starting off 
    int count = 0;

    char  * saved_string ; 
    while (stack != NULL)
    {
        DIR *dp;
        char * currentPath = pop(&stack);
        dp = opendir(currentPath);  // opening the directory
        if (dp == NULL) {
            perror("Error ");
            return;
        }
        
        while ((entry = readdir(dp)) != NULL) { 
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            snprintf(temp, sizeof(temp), "%s/%s", currentPath, entry->d_name);
            if (lstat(temp, &fileStat) == -1) {
            perror("stat");
            continue;
            }

            if (S_ISLNK(fileStat.st_mode)) {
            // Skip symbolic links to avoid loops
            continue;
            }

            if(S_ISDIR(fileStat.st_mode))
                push(&stack, temp); // if it is a directory check in it as well

            // some sort of comparision logic needs to be executed

            if(strncmp(entry->d_name, match, strlen(match)) == 0) // if the entry name and the match are exactly the same we have found out of your target 
            { // assuming exact matching for a while
               
                if(S_ISDIR(fileStat.st_mode) && checkdir != 2)
                {
                     count++;
                    saved_string = strdup(temp);
                    blue();
                    char relative_directory[MAX_SIZE];
                    displayRelative(temp, home, relative_directory);
                    printf("%s\n" , relative_directory);
                    ublue();
                }
                else if(S_ISDIR(fileStat.st_mode) == 0 && checkdir != 1) // 1 means only look for d
                {
                     count++;
                    saved_string = strdup(temp);
                    green();
                    char relative_directory[MAX_SIZE];
                    displayRelative(temp, home, relative_directory);
                    printf("%s\n" , relative_directory);
                    ugreen();
                }
            }  
        }
        
        closedir(dp);
    }
    // printf("%d", count);
    if(count ==0)
        printf("No match found!\n");

    else if(count == 1 && e_flag  == 1)
    {
        // then we need to do something
        if (lstat(saved_string, &fileStat) == -1) {
            perror("stat");
            return;
        }

        if(S_ISDIR(fileStat.st_mode) == 0) // a single file and no directory
        {
            if (access(saved_string, R_OK) != 0) {
                printf("Missing read permission for task\n");
                return;
            }
            print_file_content(saved_string);
        }
        else if(S_ISDIR(fileStat.st_mode) == 1)
        {
            if (access(saved_string, X_OK) != 0) {
                printf("Missing permissions for task!\n");
                return;
            }
            getcwd(current_working_directory, sizeof(current_working_directory));
            strncpy(prev_directory, current_working_directory, MAX_SIZE);
            printf("%s\n", saved_string);
            if (chdir(saved_string) == -1) {
                perror(saved_string);
                return;
            }
              getcwd(current_working_directory, sizeof(current_working_directory));
              // updating the previous directory
        }
    }
}
void seek_handler(char ** strings , int i)
{
    d_flag = 0;
    f_flag = 0;
    e_flag = 0;
    other_flag = 0;
    i = seek_flag_handler(strings, i); // look for flags
    if(other_flag == 1 || (d_flag == 1 && f_flag == 2))
    {
        printf("Invalid flags!\n");
        return ;
    }

    // the last argument in seek() : 0 : look for everything
    // 1 : look for directories
    // 2 : look for files only
    if(strings[i] == NULL) 
    {
        printf("Enter complete argument\n");
        return;
    }
    else  // checking for search 
    {
        char * target = strings[i]; // target you need to find 
        i++;
        char path[MAX_SIZE];
        if(strings[i] == NULL)
        { 
            getcwd(path , sizeof(path));
            seek(path, target, strdup(path), f_flag + d_flag);
        }
        // so the user has also provided some path  (with which we have to search relatively)
        else 
        {
            char * path = strings[i]; 
            // need to convert home is proper format

            if(strcmp(path , "~") == 0)  
            {
                char cwd[MAX_SIZE];
                getcwd(cwd, sizeof(cwd));  // just like home
                // printf("%s", path);
               seek(cwd, target, strdup(cwd), f_flag + d_flag);
            }
            else {

                if(strings[i][0] == '/')
                {
                    seek(strings[i], target, strdup(strings[i]), f_flag + d_flag);
                }
                else if(strings[i][0] == '~')
                {
                    char cwd[MAX_SIZE];
                    getcwd(cwd, sizeof(cwd)); 
                    // snprintf(path, sizeof(path), "%s/%s", cwd, strings[i]);
                    char temp[MAX_SIZE] = "\0";
                    strcat(temp, cwd);
                    strcat(temp, strings[i] + 1);
                    seek(temp, target, strdup(temp), f_flag + d_flag);
                }
                else 
                {
                    char cwd[MAX_SIZE];
                    getcwd(cwd, sizeof(cwd)); 
                    // snprintf(path, sizeof(path), "%s/%s", cwd, strings[i]);
                    char temp[MAX_SIZE] = "\0";
                    strcat(temp, cwd);
                    strcat(temp, "/");
                    strcat(temp, strings[i]);
                    seek(temp, target, strdup(temp), f_flag + d_flag);
                }
                // the path will have no spaces : assumption
            }
        }
    }
}