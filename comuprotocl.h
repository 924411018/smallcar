/**
* @brief	ͨ��Э����صĳ��������͡�����
* @author	hzb
* @date		2014.9.24
* @version	v1
*/

#ifndef	_COMUPROTOCL_H_
#define	_COMUPROTOCL_H_ 1

/* ��ͷ��β */
#define	FRAME_HEAD	(0x5555)
#define	FRAME_END	(0xAAAA)

/* ������ */
#define	APPROXIMATED_HOPED_PATH		(0x51)//��������·����
#define	PRACTICAL_CURVED_PATH		(0x54)//ʵ����ʻ·����
#define	SINNALLING_ CAR _2_ ANDROID	(0xA1)//С����Android��������
#define	SINNALLING_ANDROID_2_CAR	(0xA4)//Android��С����������

/* ת���� */
#define	CLOCKWISE			(0x00)//С��˳ʱ��ת��
#define	C_CLOCKWISE			(0xFF)//С����ʱ��ת��

/* �ַ������� */
#define	HELLO_CAR			("hello, car.")//ѯ��С����ϵͳ�Ƿ����
#define	STOP				("car, stop.")//ǿ������С����ϵͳֹͣ
#define	CAR_OKAY			("yes sir, I am ready.")//��ӦAndroid��APP��ѯ���Ƿ��������
#define	CAR_NOT_OKAY			("sorry sir, I am bad.")//��ӦAndroid��APP��ѯ���Ƿ��������


/* analysis_buff_recv() */
extern void analysis_buff_recv(unsigned char *buff_recv, int n, unsigned char *pchar);
#endif
