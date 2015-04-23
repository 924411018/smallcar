/**
* @name		server_setup.c
* @brief		基于天嵌科技TQ2440开发板的小车 的各子函数
* @author		hzb
* @version	v2
* @date		2014.7.17
*/
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

/**
* @brief					创建服务器
* @param fd_server_sockt		服务器socket文件描述符
* @param fd_client_sockt		客户端socket文件描述符
* @param family			socket使用的协议族
* @param type				socket类型
* @param protocol			通常为0
* @param sin_port			服务器开放的端口号
* @param backlog			允许排队等候接入的客户端数
* @param socktadd_client		客户端socket地址信息
* @param buff_recv			服务器接收数据缓冲区
*/
void server_setup(int *fd_server_sockt, int *fd_client_sockt, int family, int type, int protocol, unsigned short sin_port, int backlog, struct sockaddr_in *socktadd_client, char *buff_recv )
{
	/* creat socket */
	*fd_server_sockt = socket(family, type, protocol);
	if( *fd_server_sockt == -1 ){
		perror("create socket error");
	}else{
		printf("Success to create socket %d\n", *fd_server_sockt);
	}
	
	/* set socket property and bind it  */
	struct sockaddr_in serv_addr;
	serv_addr.sin_family=family;
	serv_addr.sin_port =htons(sin_port);
	serv_addr.sin_addr.s_addr =htons(INADDR_ANY);
	bzero(&(serv_addr.sin_zero), 8);
	if( bind(*fd_server_sockt, (struct sockaddr*)&serv_addr, sizeof(serv_addr))   != 0 )
	{
		printf("bind address fail! %d\n",errno);
		close(*fd_server_sockt);
	}else{
		printf("Success to bind address!\n");
	}

	/* listen to socket */  
	if( listen(*fd_server_sockt, backlog) != 0  )
	{
		perror("listen socket error\n");
		close(*fd_server_sockt);
	}else{
		printf("Success to listen\n");
	}

	/* accept client's visiting */
	socklen_t len;
	len = sizeof( *socktadd_client);
	*fd_client_sockt = accept(*fd_server_sockt, (struct sockaddr*)socktadd_client, &len);
	if(*fd_client_sockt <= 0 )
	{
		perror("accept() error!\n");
		close(*fd_server_sockt);
	}else{
		printf("accepted client's connection\n");
	}
	
	/* fcntl the client socket */
	if(fcntl(*fd_client_sockt,F_SETFL,O_NONBLOCK)==-1)
	{
		 perror("fcntl");
		 exit(errno);
	}
}
