
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "./rtc.h"

/*
#define sec(val)  (val>>4) & 0x7
#define min(val)  (val>>4) & 0x7
#define hour(val) (val>>4) & 0x3
#define day(val)  (val>>4) & 0x3
#define week(val) val & 0x3
#define mon(val)  (val>>4) & 0x1
#define year(val) (val) & 0xff
*/
int main(void)
{

	int fd;
	struct rtc_time time;

	fd = open("/dev/myrtc", O_RDWR);
	if (fd < 0) {
		perror("open call");
		return -1;
	}

	read(fd, &time, sizeof(time));
	
	printf("sec : %d\n", time.sec);
	printf("min : %d\n", time.min);
	printf("hour: %d\n", time.hour);
	printf("day : %d\n", time.day);
	printf("mon : %d\n", time.mon);
	printf("year: %d\n", time.year);

	sleep(1);
	close(fd);
	return 0;
}


