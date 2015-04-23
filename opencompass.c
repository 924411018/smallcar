/*
* @name		opencompass.c
* @brief		初始化电子罗盘子函数，为其读写做好准备
* @author		hzb
* @version	v2
* @date		2014.7.17
*/
#include <stdio.h>

int opencompass(int *fd_seri)
{
	/* open compass sersor */
	if( ( *fd_seri = opencom(2) ) == -1 )
	{
		perror("open /dev/ttySAC2 err.\n");
		return -1;
	}else{
		printf("open /dev/ttySAC2 suc.\n");
	}

	/* setup communicaton parameters of /dev/ttySAC2  */
	if ( setcom(*fd_seri, 9600, 8, 'N', 1) == -1 )
	{
		perror("setcom err.\n");
		return -1;
	}else{
		printf("setcom suc.\n");
		return 0;
	}
}
