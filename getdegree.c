/*
* @name		getdegree.c
* @brief		电子罗盘角度获取子函数
* @author		hzb
* @version	v2
* @date		2014.7.17
*/
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>

int getdegree(int *fd_seri, float *degre)
{
	/* send 0x31(ASCII of character '1') to get current degree */
	char frame[8];
	float degree;
	int  i;
	if ( write(*fd_seri, "1", 1) != 1 )
	{
		perror("send 0x31 err.\n");
	}else{
//		printf("send 0x31 suc.waiting result...\n");
		memset(frame, 0, 8);
		i = read(*fd_seri, frame, 8);
		if( i == 8)
		{
//			printf("get current degree suc.\n");
			degree = 0;
			
			/* 获取角度百位数值 */
			switch(frame[2])
			{
				case 0x30:break;
				case 0x31:
						degree += 100.0;break;
				case 0x32:
						degree += 200.0;break;
				case 0x33:
						degree += 300.0;break;
				default:
						degree += 0;break;
			}
			
			/* 获取角度十位数值 */
			switch(frame[3])
			{
				case 0x30:break;
				case 0x31:
						degree += 10.0;break;
				case 0x32:
						degree += 20.0;break;
				case 0x33:
						degree += 30.0;break;
				case 0x34:
						degree += 40.0;break;
				case 0x35:
						degree += 50.0;break;
				case 0x36:
						degree += 60.0;break;
				case 0x37:
						degree += 70.0;break;
				case 0x38:
						degree += 80.0;break;
				case 0x39:
						degree += 90.0;break;
				default:
						degree += 0;break;
			}
			
			/* 获取角度个位数值 */
			switch(frame[4])
			{
				case 0x30:break;
				case 0x31:
						degree += 1.0;break;
				case 0x32:
						degree += 2.0;break;
				case 0x33:
						degree += 3.0;break;
				case 0x34:
						degree += 4.0;break;
				case 0x35:
						degree += 5.0;break;
				case 0x36:
						degree += 6.0;break;
				case 0x37:
						degree += 7.0;break;
				case 0x38:
						degree += 8.0;break;
				case 0x39:
						degree += 9.0;break;
				default:
						degree += 0;break;
			}
			
			/* 获取角度小数位数值 */
			switch(frame[6])
			{
				case 0x30:break;
				case 0x31:
						degree += 0.1;break;
				case 0x32:
						degree += 0.2;break;
				case 0x33:
						degree += 0.3;break;
				case 0x34:
						degree += 0.4;break;
				case 0x35:
						degree += 0.5;break;
				case 0x36:
						degree += 0.6;break;
				case 0x37:
						degree += 0.7;break;
				case 0x38:
						degree += 0.8;break;
				case 0x39:
						degree += 0.9;break;
				default:
						degree += 0;break;
			}
			
//			printf("[current degree]:%5.1f\n", degree);
			*degre = degree;
			return 0;
		}else{
			printf("compass result returned format err.\n");
		}
	}
	
	/* getdegree err */
	return -1;	
}
