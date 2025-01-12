#ifndef INPUT_H
#define INPUT_H

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include "execute.h"
# include "background.h"
# include "initialize.h"
# include "activities.h"
# include <ctype.h>


extern void get_input(char * input);
extern void  split_on_semicolon(char *input);
#endif