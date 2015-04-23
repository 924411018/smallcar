/*
* @brief		校正电子罗盘
* @fd_seri		电子罗盘对应的串口的文件描述符
* @fd_motor		电机对应的文件描述符
* @return		0~9:suc.	-1:error
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int recorrect_compass(int *fd_seri, int fd_motor)
{
	/* send 0xc0 to recorrect magnetic field */
	char frame[8];
	char sta_correct = 0xc0;
	char end_correct = 0xc1;
	int  i = 0;
	memset(frame, 0, 8);
	if ( write( *fd_seri, &sta_correct, 1) != 1 )
	{
		perror("send 0xc0 err1.\n");
		exit(0);
	}
	else
	{
		/* 若GY-26正确识别了已发的校准命令，则应回复0X0D 0X0A 0x30 0x30 0x30 0X2E 0x30 0X05 */
		if(read( *fd_seri, frame, 8) != 8){
//			perror("send 0xc0 err2.\n");
//			exit(0);
		}
		if( frame[7] != (char)0x05){
//			perror("send 0xc0 err3.\n");
//			exit(0);
		}
		
		/* The car will circle itself to accomplish recorrection */
		printf("recorrecting the compass...more than 60 sec will be taked.(normally, led on it is on.)...\n");
		while( i < 1150 )//1125~1200
		{
			ioctl(fd_motor, 5, 0);//circle wisely
			usleep(300);//sleep 1ms
			ioctl(fd_motor, 3, 0);//stop
			usleep(300);//sleep 52ms
			i++;
		}
		i = 0;
		
		/* send 0xc1 to finish compass correcting */
		write( *fd_seri, &end_correct, 1);
		memset(frame, 0, 8);
		i = read( *fd_seri, frame, 8);
		if( i == 8)
		{
			switch(frame[6])//越大，校正效果越好
			{
				case 0x30:
						i = 0;break;
				case 0x31:
						i = 1;break;
				case 0x32:
						i = 2;break;
				case 0x33:
						i = 3;break;
				case 0x34:
						i = 4;break;
				case 0x35:
						i = 5;break;
				case 0x36:
						i = 6;break;
				case 0x37:
						i = 7;break;
				case 0x38:
						i = 8;break;
				case 0x39:
						i = 9;break;
				default:
						i = 0;break;
			}
			/* 此次校准效果不佳时，恢复出厂设置 */
			if( i < 5 )
			{
					
			}
			return i;
		}else{
			printf("compass result returned format err.\n");
			return -1;
		}
	}
}
