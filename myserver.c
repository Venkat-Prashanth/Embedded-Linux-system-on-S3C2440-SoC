/* 
 * Example showing how to implement tcp/ip server.
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
#include <signal.h>
  
#define PATH "/tmp/"
  void
sighand (int signum) 
{
  wait (NULL);
} 
int

main ()
{
  int listfd, connfd, retval;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  signal (SIGCHLD, sighand);
  listfd = socket (AF_INET, SOCK_STREAM, 0);
  if (listfd < 0)
    {
      perror ("sock:");
      exit (1);
    }
  bzero (&servaddr, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (8000);
	retval = bind (listfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
  if (retval < 0)
    {
      perror ("bind:");
      exit (2);
    }
  listen (listfd, 5);
  while (1)
    {
      char buf[200];
      char file[200];
      pid_t p;
      int n, ret, count;
      int fd;
      clilen = sizeof (cliaddr);
      connfd = accept (listfd, (struct sockaddr *) &cliaddr, &clilen);
      printf (" client connected \n");
      p = fork ();
      if (p == 0)
	{
	  close (listfd);
	  n = read (connfd, buf, 200);
	  buf[n] = '\0';
	  strcpy (file, PATH);
	  strcat (file, buf);
	  printf ("%s\n", file);
	  ret = access (file, F_OK);
	  if (!ret)
	    {
	      fd = open (file, O_RDONLY);	// Opening a file with READ/WRITE permissions
	      while (1)
		{
		  count = read (fd, buf, 199);	// Returns number of bytes read
		  buf[count] = '\n';
		  if (count == 0)
		    {
		      write (connfd, "0", 2);
		      break;
		    }
		  write (connfd, buf, count);
		}
	   }
	  
	  else
	    write (connfd, "File Not Found", 14);
	}
      
	//printf(" New File Desp : %d\n",fd);
	exit (0);		// terminate child.
      close (connfd);
    }
  exit (0);
}


