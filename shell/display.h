#ifndef DISPLAY_H
#define DISPLAY_H

# include <stdio.h> 
# include "color.h"
# include "initialize.h"
extern void display_foreground(char * commands, int time);
extern void display();
#endif

// struct passwd {
//     char   *pw_name;       /* username */
//     char   *pw_passwd;     /* user password */
//     uid_t   pw_uid;        /* user ID */
//     gid_t   pw_gid;        /* group ID */
//     char   *pw_gecos;      /* user information */
//     char   *pw_dir;        /* home directory */
//     char   *pw_shell;      /* shell program */
// };