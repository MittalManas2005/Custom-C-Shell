# ifndef MAN_H
# define MAN_H

# include <stdio.h>
# include <netinet/in.h> // contains sockaddr_in
# include <sys/socket.h>
# include <sys/types.h>
# include <arpa/inet.h>  
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <netdb.h>

extern int iMan_handler(char ** strings, int index);
# endif