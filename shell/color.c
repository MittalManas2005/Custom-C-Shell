# include "color.h"
void blue()
{
    printf("\033[0;34m");
}
void ublue()
{
    printf("\033[0m");
}
void green()
{
    printf("\033[0;32m");
}
void ugreen()
{
    printf("\033[0m");
}

void red()
{
    printf("\033[0;31m");
}
void ured()
{
    printf("\033[0m");
}

void show(char * s, int color) // 1 : blue , 2 : green , 3 : red
{
    if(color == 0)
        printf("%s\n", s);
    else if(color == 1)
    {
        blue();
        printf("%s\n", s);
        ublue();
    }
    else if(color == 2)
    {
        green();
        printf("%s\n", s);
        ugreen();
    }
    else if(color == 3)
    {
        red();
        printf("%s\n", s);
        ured();
    }
}


