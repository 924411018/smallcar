/**
* @brief	通信协议相关的常量、类型、声明
* @author	hzb
* @date		2014.9.24
* @version	v1
*/

#ifndef	_COMUPROTOCL_H_
#define	_COMUPROTOCL_H_ 1

/* 侦头侦尾 */
#define	FRAME_HEAD	(0x5555)
#define	FRAME_END	(0xAAAA)

/* 侦类型 */
#define	APPROXIMATED_HOPED_PATH		(0x51)//近似期望路径侦
#define	PRACTICAL_CURVED_PATH		(0x54)//实际行驶路径侦
#define	SINNALLING_ CAR _2_ ANDROID	(0xA1)//小车到Android的信令侦
#define	SINNALLING_ANDROID_2_CAR	(0xA4)//Android到小车的信令侦

/* 转向方向 */
#define	CLOCKWISE			(0x00)//小车顺时针转向
#define	C_CLOCKWISE			(0xFF)//小车逆时针转向

/* 字符串常量 */
#define	HELLO_CAR			("hello, car.")//询问小车子系统是否就绪
#define	STOP				("car, stop.")//强行命令小车子系统停止
#define	CAR_OKAY			("yes sir, I am ready.")//回应Android端APP的询问是否就绪信令
#define	CAR_NOT_OKAY			("sorry sir, I am bad.")//回应Android端APP的询问是否就绪信令


/* analysis_buff_recv() */
extern void analysis_buff_recv(unsigned char *buff_recv, int n, unsigned char *pchar);
#endif
