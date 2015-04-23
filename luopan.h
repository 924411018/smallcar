/*
* @name		luopan.h
* @brief		基于天嵌科技TQ2440开发板的小车 电子罗盘相关子函数的头文件
* @author		hzb
* @version	v2
* @date		2014.7.17
*/

#ifndef _LUOPAN_H_
#define _LUOPAN_H_

/* functions declare */
extern int getdegree(int *fd_seri, float *degre);
extern int opencom( int com_port);
extern int opencompass(int *fd_seri);
extern int recorrect_compass(int *fd_seri, int fd_motor);
extern int setcom( int fd, int baud_rate, int data_bits, char parity, int stop_bits);

#endif
