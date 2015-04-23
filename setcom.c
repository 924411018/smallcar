/*
* @brief, subfunction to config a com
* @fd, corressponds to a tty-device nod-file
* @baud_rate, 波特率
* @data_bits, 数据位
* @parity, 校验方式
* @stop_bits, 停止位
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

int setcom( int fd, int baud_rate, int data_bits, char parity, int stop_bits)
{
	struct termios new_cfg, old_cfg;
	int speed;

	/* save and test current com configration */
	if ( tcgetattr( fd, &old_cfg)  != 0 )
	{
		perror("tcgetattr() error\n");
		return -1;	
	}		
	new_cfg = old_cfg;
	cfmakeraw( &new_cfg);
	new_cfg.c_cflag &= ~CSIZE;
	new_cfg.c_cflag |= CLOCAL | CREAD;

	/* setup baud rate */
	switch(baud_rate)
	{
		case 2400:  { speed = B2400;} break;
		case 4800:  { speed = B4800;} break;
		case 9600:  { speed = B9600;} break;
		case 19200:  { speed = B19200;} break;
		case 38400:  { speed = B38400;} break;		
		default:		
		case 115200:  { speed = B115200;} break;
	}
	cfsetispeed( &new_cfg, speed);
	cfsetospeed( &new_cfg, speed);	

	/* setup data bits */
	switch( data_bits)
	{
		case 7:	{ new_cfg.c_cflag |= CS7;}break;
		default:
		case 8:	{ new_cfg.c_cflag |= CS8;}break;
	}

	/* setup parity */
	switch( parity)
	{
		default:/* no parity */
		case 'n':
		case 'N':
		{
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_iflag &= ~INPCK;		
		}
		break;
		case 'o':
		case 'O':
		{
			new_cfg.c_cflag |=  (PARODD | PARENB);
			new_cfg.c_iflag |= INPCK;		
		}
		break;
		case 'e':
		case 'E':
		{
			new_cfg.c_cflag |= PARENB;
			new_cfg.c_cflag &= ~PARODD;
			new_cfg.c_iflag |= ~INPCK;		
		}
		break;
		case 's':
		case 'S':
		{
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_cflag &= ~CSTOPB;
		}
		break;
	}
	
	/* setup stop bits */
	switch( stop_bits)
	{
		default:
		case 1:	{ new_cfg.c_cflag &= ~CSTOPB;}break;
		case 2: { new_cfg.c_cflag |= CSTOPB;}
	}

	/* setup min wait-time and min recive-bytes */
	new_cfg.c_cc[VTIME] = 1;
	new_cfg.c_cc[VMIN] = 8;
	tcflush(fd, TCIFLUSH);

	/* activate new configration */
	if ( tcsetattr(fd, TCSANOW, &new_cfg)  != 0 )
	{
		perror("tcsetattr() error");
		return 0;
	}
}