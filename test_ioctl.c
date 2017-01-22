#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "ioctl_const.h"

int main()
{
        int fd, i;

        fd = open( "/dev/myled", O_RDWR );
        if( fd < 0 ) {
                printf("\n\nDevice could not be opened\n\n");
                return 1;
        }
        ioctl( fd, LED_ON);
	sleep(1);
        ioctl( fd, LED_OFF);
	return 0;
}
