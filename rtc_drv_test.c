#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MYRTC "/dev/rtc_driver"

void Delay_MS(unsigned int time)	// 50 ns 
{
	unsigned int i, j;
	for (i = 0; i < time; i++)
	{
		for (j = 0; j < 30000; j++) ;
	}
}

struct TIME_RTC  {
	unsigned int year;
	 unsigned int mon;
	 unsigned int day;	// Week 
	unsigned int date;
	 unsigned int hour;
	 unsigned int min;
	 unsigned int sec;
} TIME_RTC;

int main(void)
{
	int fd, i = 0;
	int cmd;
	int status;
	struct TIME_RTC rtc_time;
	printf("the struct TIME_RTC len is:%d \n", sizeof(struct TIME_RTC));
	fd = open(MYRTC, O_RDWR, 0666);
	if (fd < 0)
		 {
		perror("open device rtc_driver error \n");
		exit(1);
		}
	printf("open /dev/ rtc_driver success \n!");
	int year, mon, date, day, hour, min, sec;
	read(fd, &rtc_time, sizeof(struct TIME_RTC));
	printf("************ 000000 *************** \n");
	printf("** date:%d : %d : %d \n", rtc_time.year, rtc_time.mon,
		rtc_time.date);
	printf("** workday:%d \n", rtc_time.day);
	printf("** time:%d:%d:%d \n", rtc_time.hour, rtc_time.min, rtc_time.sec);
	close(fd);
	return 0;
}


