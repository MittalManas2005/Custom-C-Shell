# include "hop.h"
# define MAX_SIZE 4096

int hop_handler(char ** str, int index){
    
    int i = index;

    if(str[i] == NULL)
    {
        // saving cwd in previous working directory
       if (chdir(home_directory) == -1) {
            perror(home_directory);
            return -1;
        }
        strncpy(prev_directory, current_working_directory, MAX_SIZE); 
        getcwd(current_working_directory , MAX_SIZE);
        printf("%s\n", current_working_directory);
    }

    while (str[i] != NULL)
    {
        if(strcmp(str[i], "~") == 0) 
        {
            if (chdir(home_directory) == -1) {
                
                perror(home_directory);
                return -1;
            }
            strncpy(prev_directory, current_working_directory, MAX_SIZE); // saving cwd in previous working directory
            getcwd(current_working_directory , MAX_SIZE);
        }
        else if(strcmp(str[i], "-") == 0)
        {
            if (chdir(prev_directory) == -1) {
                
                perror(prev_directory);
                return -1;
            }
            strncpy(prev_directory, current_working_directory, MAX_SIZE);
            getcwd(current_working_directory, MAX_SIZE);
        }
        else if(str[i][0] == '~' && str[i][1] == '/')
        {
            char new_path[MAX_SIZE];
            snprintf(new_path, sizeof(new_path), "%s%s", home_directory, &str[i][1]);
            strncpy(prev_directory, current_working_directory, MAX_SIZE); // Save cwd
            if (chdir(new_path) == -1) {
               
                perror(new_path);
                return -1;
            }
            getcwd(current_working_directory, MAX_SIZE);
        }
        else 
        {
           if (chdir(str[i]) == -1) {
                perror(str[i]);
                return -1;
            }
            strncpy(prev_directory, current_working_directory, MAX_SIZE);
            getcwd(current_working_directory, MAX_SIZE);
        }
        printf("%s\n", current_working_directory);
        i++;
    }
    return 0;
}