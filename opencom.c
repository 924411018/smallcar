/*
* @brief				subfunction to open a device
* @parameter com_port	indicates which one serial port to be opend
* @return, -1 indicates error
*/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

int opencom( int com_port)
{
	int fd;
	char *dev[] = {"/dev/ttySAC0", "/dev/ttySAC1", "/dev/ttySAC2", "/dev/ttySAC3"};

	/* open serial port */
	fd = open(dev[com_port], O_RDWR|O_NOCTTY|O_NDELAY);
	if( fd < 0 )
	{
		perror("open serial port failed");
		return -1;
	}
	
	/* fcntl */
	if( fcntl(fd, F_SETFL, 0) < 0  )
	{
		perror("fcntl F_SETFL\n");
	}
	
	/* test whether fd is a tty or not */
	if( isatty(fd) == 0)
	{
		perror("This is not a terminal device");		
	}

	return fd;
}

