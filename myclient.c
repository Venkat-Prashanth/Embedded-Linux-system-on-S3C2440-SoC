
/*
 * Example showing how to implement tcp/ip client.
 * Version: 1.0
 * Author : Team -C
*/ 
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ioctl_const.h"
  int
main () 
{
  int fd, fd1;
  char buf[200] = "first";
  char *serv_ip = "10.0.0.4";
  int n;
  int sockfd, ret_val;
  struct sockaddr_in servaddr;
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  bzero (&servaddr, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (8000);
  inet_pton (AF_INET, serv_ip, &servaddr.sin_addr);
  fd1 = open ("/dev/myled", O_RDWR);
  if (fd1 < 0)
    {
      printf ("\n\nDevice could not be opened\n\n");
      return 1;
    }
  ret_val =
    connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
  if (ret_val < 0)
    {
      perror ("connect:");
      exit (1);
    }
  n = write (sockfd, buf, strlen (buf));
  
    //      buf[n]='\0';
    n = read (sockfd, buf, 200);
  
    // buf[n]='\0';
    if (strcmp (buf, "File Not Found") == 0)
    {
      printf ("%s\n", buf);
      exit (1);
    }
  	fd = open ("test", O_RDWR | O_CREAT, 0666);	// Opening a file with READ/WRITE permissions
  if (fd < 0)
    {
      perror ("write");
      exit (0);
    }
  write (fd, buf, strlen (buf));
  while (1)
    {
      n = read (sockfd, buf, 200);
      
//                      printf("%s\n", buf);
	//      buf[n]='\0';
	ioctl (fd1, LED_ON);
      sleep (1);
      
	//if(strcmp(buf, "0")==0) {
	if (n == 0)
	{
	  printf ("%s\n", buf);
	  close (fd);
	  ioctl (fd1, LED_OFF);
	  goto END;
	}
      write (fd, buf, strlen (buf));
      ioctl (fd1, LED_OFF);
      sleep (1);
    }
	END:
	close (sockfd);
  return 0;
	}

