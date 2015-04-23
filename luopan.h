/*
* @name		luopan.h
* @brief		������Ƕ�Ƽ�TQ2440�������С�� ������������Ӻ�����ͷ�ļ�
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
