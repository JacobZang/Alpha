#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char **argv)
{
    int fd, retvalue;
    char *filename;
    char databuf[1];

    if(argc != 3)
    {
        printf("Usage: \n");
        printf("%s <dev> [string]", argv[0]);
        return -1;
    }
    
    filename = argv[1];

    fd = open(filename, O_RDWR);
    if(fd < 0)
    {
        printf("Error opening\r\n");
        return -1;
    }

    databuf[0] = atoi(argv[2]);

    retvalue = write(fd, databuf, sizeof(databuf));
    if(retvalue < 0)
    {
        printf("LED control failed\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}