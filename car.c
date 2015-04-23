
/*
* @name			car.c
* @brief 		Ð¡³µÉÏÔËÐÐµÄmain³ÌÐò¡£
* @author		hzb
* @version		v2
* @date			2014.9.22
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "luopan.h"
#include "smacar.h"
#include "comuprotocl.h"

int main(int argc, char *argv[])
{
	/* open device compass */
	int fd_compass;
	if(opencompass( &fd_compass ) != 0 )
	{
		perror("compass sensor initialization err. has to exits.\n");
		exit(0);
	}
	
	/* open device /dev/motor */
	int fd_motor;
	fd_motor = open("/dev/motor",0);
	if(fd_motor < 0)
	{
	  perror("open device EmbedSky_motor error\n");
	  exit(1);
	}
	printf("EmbedSky_motor success\n");
	
	/* sleep 1 seconds */
	sleep(1);
	
	/* recorrect the compass */
//	int range;
//	range = recorrect_compass(&fd_compass, fd_motor);
//	if( range == -1)
//	{
//		perror("recorrect compass err. degree result returned form it may be uncorrect.\n");	
//	}else{
//		printf("recorrect compass suc.\n");
//		printf("[recorrecting range]:%d\n", range);
//	}
	
	/* create shared memory */
	int shm_id;
	shm_id = shmget(IPC_PRIVATE, 1024, 0666);
	if(shm_id < 0){perror("get shared memory err.\n");
	}else{
		printf("get shared memory suc.%d\n", shm_id);
	}
	
	/* ŽŽœš×Óœø³Ì£¬ÔÚ×Óœø³ÌÖÐÊµÏÖÀï³ÌŒÆÊý */
	pid_t pid_licheng;
	pid_licheng = fork();
	if(pid_licheng < 0){
		perror("create child process err.can't get distance\n");
		exit(1);
	}
	else if(pid_licheng == 0){
			/* ×Óœø³ÌÒýÈë¹²ÏíÄÚŽæ */
			char *shmaddr;
			int *dis;
			if( ( shmaddr=(char *) (shmat(shm_id, 0, 0) ) ) < (char *)0 ){
				perror("shmat() in licheng process err.\n");
				exit(1);
			}else{
				dis = (int *)shmaddr;
			}
		
			/* Àï³ÌŒÆŒÆÊý */
			int button_fd;
			fd_set rds;
			int ret, key_value;
			button_fd=open("/dev/IRQ-Test",0);
			if(button_fd<0)
			{
				perror("open device button err.\n");
				exit(1);
			}
			while(1)
			{
				FD_ZERO(&rds);
				FD_SET(button_fd,&rds);
				ret=select(button_fd+1,&rds,NULL,NULL,NULL);
				if(ret<0){perror("select");return -1;}
				if(ret==0){printf("Timeout.\n");}
				else if(FD_ISSET(button_fd,&rds)){
					int ret=read(button_fd,&key_value,sizeof(key_value));
					if(ret !=sizeof(key_value))
					{
						if(errno !=EAGAIN)perror("read button\n");
						continue;
					}else{
						(*dis)++;
						//printf("dis=%d\n", *dis);
					}
				}
			}
	}else{
		/* ÔÚžžœø³ÌÖÐÔÙŽÎŽŽœš×Óœø³Ì£¬ÔÚ×Óœø³ÌÖÐÊµÏÖÊÓÆµÒµÎñ£¬ÔÚžžœø³ÌÖÐŽŠÀíµç»ú¿ØÖÆÒµÎñ  */
		pid_t pid_video;
		pid_video = fork();
		if(pid_video < 0){
			perror("create child process err.can't send video.\n");
		}
		else if(pid_video == 0){
			/* ÖŽÐÐ°åÔØµÄuvc_stream³ÌÐò£¬ŽŠÀíÊÓÆµÒµÎñ */
			system("/sbin/uvc_stream");			
			exit(0);
		}else{
			/* žžœø³ÌÒýÈë¹²ÏíÄÚŽæ */
			char *shmaddr;
			int *dis;//里程计总计数(包含转弯)
			int tempdis;//save *dis
			int sumdis;//save straight distance in 似期望路径侦
			int dis_run;//
			float target_orientations[1000];
			float currentdegree;
			int target_shifts[1000];
			enum move action;

			/* convert shared memory to *dis */
			if(( shmaddr=(char *) (shmat(shm_id, 0, 0) ) ) < (char *)0 ){
				perror("shmat() in licheng process err.\n");
				exit(1);
			}else{
				dis = (int *)shmaddr;
			}
			
			/* call server_setup() subfunction œšÁ¢·þÎñÆ÷*/
			int fd_server_socket, fd_client_sockt, backlog;
			struct sockaddr_in socktaddr_client;
			unsigned char *buff_recv;
			buff_recv = (unsigned char *) calloc(4008, sizeof(char) );//Ä¬ÈÏÕÛµãÊý²»³¬¹ý1000žöµã
			server_setup(&fd_server_socket, &fd_client_sockt, AF_INET, SOCK_STREAM, 0, SERV_PORT, MAX_CLIENT_NUM, &socktaddr_client, buff_recv);
			
			/* control the motor's actions according to command-data from android app */
			int i, n;
			unsigned char *path_frame;
			path_frame = (unsigned char *) calloc(4002, sizeof(char) );
			while(1)
			{
				memset(buff_recv, 0, 4008);
				memset(path_frame, 0, 4002);
				sumdis = 0;
				if( ( n=recv(fd_client_sockt, buff_recv, 4008, 0) ) > 0 )
				{
					/* print the recieved bytes */
					printf("\n[received %d bytes]:\n", n);
					for(i=0; i < n; i++)
					{
						printf("%X ", *(buff_recv + i) );//buff_recv内容以16进制形式打印
					}
					fflush(stdout);

					/* 解析buff_recv中的侦结构到path_frame中 */
					analysis_buff_recv(buff_recv, n, path_frame);

					/* 解析出近似期望路径侦，自主完成路径规划 */
					if( *(path_frame) == (unsigned char)0x81 )
					{
						/* clear *dis and sumdis */
						*dis = 0;
						sumdis = 0;				
						
						/* 打印出路径规划信息 */						
						n = *(path_frame+1)*256 + *(path_frame+2);
						printf("\n[featured point data]:\n");
						for(i=0; i < n; i++)
						{	
							printf("(");	
							*(path_frame + 3 + 4*i) == CLOCKWISE ? printf("+%d,", *(path_frame + 3 + 4*i+1) ) 
											     : printf("-%d,", *(path_frame + 3 + 4*i+1) );
							printf("%d", *(path_frame + 3 + 4*i +2) *256 + *(path_frame + 3 + 4*i +3) );
							printf(") ");
						}
						printf("\n\n");
						fflush(stdout);

						/* 确定各点处的罗盘度数值 */
						memset(target_orientations, 0, sizeof(float) * 1000);
						getdegree(&fd_compass, &currentdegree);
						currentdegree = 0;//make car's initial degree is 0;
						for(i=0; i < n; i++)
						{
							int j;
							for(j=0; j <= i; j++)
							{
								if(*(path_frame + 3 + 4*j) == CLOCKWISE)
								{
									target_orientations[i] += *(path_frame+3+4*j+1);
								}
								else
								{
																
									target_orientations[i] -= *(path_frame+3+4*j+1);
								}
							}
							target_orientations[i] += currentdegree;
							if(target_orientations[i] < 0)
							{
								target_orientations[i] += 360;
							}
							else if(target_orientations[i] > 360)
							{
								target_orientations[i] -= 360;
							}						
						}

						/* 确定各点处的前进距离 */
						memset(target_shifts, 0, sizeof(int) * 1000);
						for(i=0; i < n; i++)
						{
							target_shifts[i] = *(path_frame+3+4*i+2) * 256 + *(path_frame+3+4*i+3);
						}

						/* 定向定距 */
						float diff_degree;
						getdegree( &fd_compass, &currentdegree);
						usleep(20);
						getdegree( &fd_compass, &currentdegree);
						usleep(20);
						getdegree( &fd_compass, &currentdegree);
						printf("[orignal orientation]:%5.1f\n\n", currentdegree);
						fflush(stdout);
						report_status(fd_client_sockt, currentdegree, sumdis);
						for(i=0; i < n; i++)
						{
							/* go to target orientation */
							printf("[target orientation]:%5.1f\n", target_orientations[i]);
							fflush(stdout);
							while( 1 )
							{
								getdegree( &fd_compass, &currentdegree);
								usleep(20);
								getdegree( &fd_compass, &currentdegree);
								printf("[current orientation]:%5.1f\r", currentdegree);
								fflush(stdout);
								report_status(fd_client_sockt, currentdegree, sumdis);

								/* R or L, and update diff_degree */
								if( target_orientations[i] > currentdegree )
								{
										diff_degree = target_orientations[i] - currentdegree;
										if(diff_degree > 180){
											action = L;/* target_orientations[i] is more 180 larger than currentdegree */ 
											diff_degree = (float)360 - diff_degree;
										}else{
											action = R;/* target_orientations[i] is larger than currentdegree but no more 180 */
										}
								}
								else
								{
										diff_degree = currentdegree - target_orientations[i];
										if(diff_degree > 180){
											action = R;/* currentdegree is more 180 larger than target_orientations[i] */
											diff_degree = (float)360 - diff_degree;
										}else{
											action = L;/* currentdegree is larger than target_orientations[i] but no more 180*/
										}
								}
								
								/* if diff_degree is less than 3 */
								if(diff_degree < 3){
											
									/* diff_degree > 1.9, then car action smothly */
									while( diff_degree > 1.9f ){
										getdegree( &fd_compass, &currentdegree);
										printf("[current orientation]:%5.1f\r", currentdegree);
										fflush(stdout);
										report_status(fd_client_sockt, currentdegree, sumdis);
										if( target_orientations[i] > currentdegree )
										{
												diff_degree = target_orientations[i] - currentdegree;
												if(diff_degree > 180){
													action = L;/* target_orientations[i] is more 180 larger than currentdegree */ 
													diff_degree = (float)360 - diff_degree;
												}else{
													action = R;/* target_orientations[i] is larger than currentdegree but no more 180 */
												}
										}
										else
										{
												diff_degree = currentdegree - target_orientations[i];
												if(diff_degree > 180){
													action = R;/* currentdegree is more 180 larger than target_orientations[i] */
													diff_degree = (float)360 - diff_degree;
												}else{
													action = L;/* currentdegree is larger than target_orientations[i] but no more 180*/
												}
										}

										ioctl(fd_motor, action, 0);
										usleep(1500);//1500ms
										action = S;
										ioctl(fd_motor, action, 0);
									}
				
									/* diff_degree <= 1.9, break */
									break;			
								}
 
								/* else car action largely */
								else{
									tempdis = *dis;
									ioctl(fd_motor, action, 0);
									while(*dis < tempdis + 2);
									action = S;
									ioctl(fd_motor, action, 0);
								}
							}
							action = S;
							ioctl(fd_motor, action, 0);
							getdegree( &fd_compass, &currentdegree);
							usleep(10);
							getdegree( &fd_compass, &currentdegree);
							usleep(10);
							getdegree( &fd_compass, &currentdegree);
							printf("[current orientation]:%5.1f\n", currentdegree);
							fflush(stdout);


							/* go to target shift */
							printf("[target shift]:%d\n", target_shifts[i]);
							fflush(stdout);
							tempdis = *dis;							
							action = F;
							ioctl(fd_motor, action, 0);
							while(*dis < tempdis + target_shifts[i] -10){
								dis_run = *dis - tempdis;
								printf("[current shifs]:%d\r", dis_run );
								fflush(stdout);
								action = F;
								ioctl(fd_motor, action, 0);
								sumdis += dis_run;
								report_status(fd_client_sockt, currentdegree, sumdis);
								sumdis -= dis_run;
							}
							action = S;
							ioctl(fd_motor, action, 0);
							action = F;
							ioctl(fd_motor, action, 0);
							usleep(400);//400ms
							action = S;
							ioctl(fd_motor, action, 0);
							action = F;
							ioctl(fd_motor, action, 0);
							usleep(200);//200ms
							action = S;
							ioctl(fd_motor, action, 0);
							dis_run = *dis - tempdis;
							printf("[current shifs]:%d\r", dis_run );
							fflush(stdout);
							sumdis += dis_run;
							report_status(fd_client_sockt, currentdegree, sumdis);

							/* 每完成一点，打印一条提示 */
							printf("The featured point %d has been finished\n\n", i);
							fflush(stdout);
						}
					}

					/* 解析出遥控侦，进行手动遥控 */
					else if( *(path_frame) == (unsigned char)0x21 )
					{
						if(*(path_frame+1) == (unsigned char)'S')
						{
							ioctl(fd_motor, 3, 0);
						}
						if(*(path_frame+1) == (unsigned char)'F')
						{
							ioctl(fd_motor, 1, 0);
						}
						if(*(path_frame+1) == (unsigned char)'B')
						{
							ioctl(fd_motor, 2, 0);
						}
						if(*(path_frame+1) == (unsigned char)'L')
						{
							ioctl(fd_motor, 4, 0);
						}
						if(*(path_frame+1) == (unsigned char)'R')
						{
							ioctl(fd_motor, 5, 0);
						}
					}

					/* 其他侦，便于扩展 */
					else
					{
					
					}
				}
			}
			
			/* close the socket fd */
			close(fd_client_sockt);
			close(fd_server_socket);
		}
	}
	
	/* app ends normally */	
	return 0;
}
