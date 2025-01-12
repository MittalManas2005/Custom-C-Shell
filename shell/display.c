# include "display.h"

void display()
{
    blue();
    calculateRelativeDirectory(current_working_directory, home_directory, relative_directory);
    printf("\033[1m<%s@%s:%s> \033[0m", userName, systemName, relative_directory);
    ublue();
}
void display_foreground(char * commands, int time)
{
    blue();
    calculateRelativeDirectory(current_working_directory, home_directory, relative_directory);
    printf("\033[1m<%s@%s:%s %s : %d> \033[0m", userName, systemName, relative_directory, commands, time);
    ublue();
}