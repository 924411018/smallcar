/**
* @brief	Andorid端APP下发数据时，小车子系统在Socket缓冲数据流中有对应的数据，本函数对缓冲数据进行分析，
*		提取出对应的侦结构，并检查侦的完好性。
* @buff_recv	指向缓冲区的的指针
* @n		缓冲区长度
* @pchar	指向提取的侦结构的侦头
* @return	指向一片内存的指针，第一字节表示该函数返回的状态，往后字节表示随第一字节返回的附加数据，因第一字节的不同而有不同含义，详见函数内部关于返回值的注释
*		若*pchar == 0x21，则是手动遥控侦；其后跟一个字节的表示动作控制的字符————’即*（pchar+1）取F‘、’B‘、’L‘、’R‘、‘S’之一。
*		若*pchar == 0x81, 则是成功的近似期望路径侦；*（pchar+1）表示角度-距离对的个数高字节，*（pchar+2）表示角度-距离对的个数低字节，随后跟各个角度-距离对的具体数值。
*		若*pchar == 0x23，则该侦存在错误；随后附加字节数为0 
* @author	hzb
* @version	v1
* @date		20140927
*/

#include "comuprotocl.h"
#include <string.h>
#include <stdio.h>

void analysis_buff_recv(unsigned char *buff_recv, int n, unsigned char *pchar)
{
	/* 缓冲区长度小于8，则认为是手动遥控下发的数据 */
	if(n < 8 )
	{
		if( *(buff_recv+n-1) == (unsigned char)'S') {*pchar = (unsigned char)0x21; *(pchar+1) = 'S';}
		if( *(buff_recv+n-1) == (unsigned char)'B') {*pchar = (unsigned char)0x21; *(pchar+1) = 'B';}
		if( *(buff_recv+n-1) == (unsigned char)'L') {*pchar = (unsigned char)0x21; *(pchar+1) = 'L';}
		if( *(buff_recv+n-1) == (unsigned char)'R') {*pchar = (unsigned char)0x21; *(pchar+1) = 'R';}
		if( *(buff_recv+n-1) == (unsigned char)'F') {*pchar = (unsigned char)0x21; *(pchar+1) = 'F';}
//		printf("n < 8\n");
		return;
	}

	/* 若缓冲区第一字节和第二字节为0x55,第三字节为0x51, 且n-8能被4整除，则认为是近似期望路径侦 */	
	else if(*buff_recv == (unsigned char)0x55 && *(buff_recv+1) == (unsigned char)0x55 && *(buff_recv+2) == (unsigned char)0x51 && (n-8) % 4 == 0)
	{
		/* 计算校验和 */
		unsigned int i, j;
		unsigned char checksum;		
		j = *(buff_recv + 3) *256 + *(buff_recv + 4);//角度距离对的个数	
		checksum = 0;
		for(i=0; i < 4 * j; i++)
		{
			checksum += *(buff_recv + 5 + i);
		}
		
		/* 若校验和正确 */
		if( checksum == *(buff_recv + n -3) )
		{
			*pchar = (unsigned char)0x81;
			*(pchar+1) = *(buff_recv + 3);
			*(pchar+2) = *(buff_recv + 4);
			for(i=0; i < 4 * j; i++)
			{
				*(pchar + 3 + i) = *(buff_recv + 5 + i);
			}			
		}
		
		/* 校验和不正确 */
		else
		{
			printf("jiaodujuli=%d, checksum=%d, checksumT=%d\n", j, checksum, *(buff_recv+n-3) );
		}
		
		return;		
	}

	/* 定位侦头，提取侦结构 */
	else
	{
		int i;		
		for(i=0; i < n; i++)
		{
			/* 检测侦头 */
			if( *( (unsigned short int *)(buff_recv + i) )  == FRAME_HEAD )
			{
//				printf("FRAME_HEAD[%X] detected at index %d\n", *( (unsigned short int *)(buff_recv + i) ), i);
			
				/* 若是近似期望路径侦 */
				if( *(buff_recv + i + 2) == APPROXIMATED_HOPED_PATH)
				{
//					printf("Frame type:%X\n", *(buff_recv + i + 2));								

					n = *(buff_recv + i + 3) *256 + *(buff_recv + i + 4);//角度距离对的个数
//					printf("find %d special point\n", n);
					if( *( (unsigned short int *)(buff_recv + i + 7 + 4 *n) )  == FRAME_END)
					{
//						printf("FRAME_END detected at index %d\n", i+7 +4 * n);									
						/* 核对校验和 */
						buff_recv += (i+5);//使buff_recv指向第一个角度距离对的第一字节
						for(i=1; i < 4 * n; i++)
						{
							*buff_recv += *(buff_recv + i);
						}
						if(*buff_recv == *(buff_recv + 4 * n) )//校验和正确
						{
//							printf("checksum ok:%X\n", *buff_recv);
							/* 将数据侦提取到pchar所指向的内存空间中 */
							*pchar = (unsigned char)0x81;
							*(pchar+1) = n/256;
							*(pchar+2) = n%256;
							for(i=0; i < 4 * n; i++)
							{
								*(pchar+3+i) = *(buff_recv + i);
							}
						}
						else
						{//校验和错误
//							printf("checksum error:should be %X, not %X\n", *buff_recv, *(buff_recv + 4 * n));
							break;
						}
					}
					else
					{//侦尾错误
//						printf("FRAME_END error.\n");									
						break;
					}
				}

				/* 不是近似期望路径侦则忽略该侦 */
				else
				{
					break;
				}					
			}
		}										
	}
	
	/* 函数返回 */
	return;
}
