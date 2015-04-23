/**
* @name		smacar.h
* @brief		������Ƕ�Ƽ�TQ2440�������С��  ���������ͷ�ļ�
* @author		hzb
* @version	v2
* @date		2014.7.17
*/
#ifndef _SMACAR_H
#define _SMACAR_H	1

/* С������Զ���н������źŵĶ˿ں� */
#define SERV_PORT	8081
/* �������Ŀͻ����� */
#define MAX_CLIENT_NUM	5

/* enum type----presents orientation of moving  */
enum move{F=1, B=2, S=3, L=4, R=5};

/* ���������ú��� */
extern void server_setup(int *fd_server_sockt, int *fd_client_sockt, int family, int type, int protocol, unsigned short sin_port, int backlog, struct sockaddr_in *socktadd_client, char *buff_recv );

/* report car's status---degree and shift */
extern int report_status(int fd_sockt_to, float curent_degre, int sum_shift);
#endif
