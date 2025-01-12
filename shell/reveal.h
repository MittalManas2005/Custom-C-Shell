 #ifndef REVEAL_H
#define REVEAL_H
# include <stdio.h>
# include "initialize.h"
# include <string.h>
# include <errno.h>
# include <unistd.h>
# include <dirent.h>
# include "color.h"
# include <fcntl.h>
# include <stdlib.h>
# include <pwd.h>
# include <grp.h>
#include <sys/xattr.h>
# include <time.h>
#include <sys/stat.h>

extern int reveal_handler(char ** strings, int index);
#endif


// Important data structures used
/*
struct dirent {
    ino_t          d_ino;       // inode number
    off_t          d_off;       // offset to the next dirent
    unsigned short d_reclen;    // length of this record
    unsigned char  d_type;      // type of file
    char           d_name[256]; // filename (null-terminated)
}
struct stat {
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	mode_t st_attr;
};
*/
