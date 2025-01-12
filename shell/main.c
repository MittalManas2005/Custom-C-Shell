# include "main.h"

# define MAX_SIZE 4096
int main()
{  
    flag_display_foreground = 0;
    initialize();
    log_initialize(); // loading the file 
    
    char  * str = malloc(MAX_SIZE * sizeof(char)); //allocated memory to str
    if(str == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1; // Exit if memory allocation fails
    }

    while (1) 
    {
        if(flag_display_foreground == 0 )
        {
            display();
            sigint_received = 0;
            sigtstp_received = 0;
        }
        else flag_display_foreground = 0;
        
        for (int i = 0; i < message_count; i++)
        {
            printf("%s", message_queue[i]);
        }
        message_count = 0;
        fflush(stdout);
        get_input(str); // get the input string
       
        split_on_semicolon(str); // separating the commands based on the semicolong
    }
    free(str);// after completion
    return 0;   
}