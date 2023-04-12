#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"

static char usrdata[] = {"usr data"};

int main(int argc, char **argv)
{
    int fd, retvalue;
    char *filename;
    char readbuf[100],writebuf[100];

    if(argc != 3)
    {
        printf("Usage: error\r\n");
        return -1;
    }
    
    filename = argv[1];

    fd = open(filename, O_RDWR);
    if(fd < 0)
    {
        printf("Error opening\r\n");
        return -1;
    }

    if(atoi(argv[2]) == 1)
    {
        retvalue = read(fd, readbuf, 100);
        if(retvalue < 0)
        {
            printf("Error reading\r\n");
            return -1;
        }
        else
        {
            printf("Success read: %s\r\n", readbuf);
        }
    }
    if(atoi(argv[2]) == 2)
    {
        memcpy(writebuf, usrdata, sizeof(usrdata));
        retvalue = write(fd, writebuf, 100);
        if(retvalue < 0)
        {
            printf("Error writing\r\n");
        }
        else
        {
            printf("Success writing\r\n");
        }
    }
    // retvalue = close(fd);
    // if(retvalue < 0)
    // {
    //     printf("Can't close file\r\n");
    //     return -1;
    // }
    return 0;
}