/*
* @name				report_status.c
* @author			hzb
* @version			v1
* @date				2014.10.7
*/

#include <stdio.h>

/**
* @brief 			report the car's current degree and straight shift to Android APP
* @fd_sockt_to		client's socket fd.
* @curent_degree	current degree from luopan
* @sum_shift		tht sum of straight shift
* @return			value of send() returned
*/
int report_status(int fd_sockt_to, float curent_degre, int sum_shift)
{
	unsigned char buff_send[10];
	
	/* frame henad */
	buff_send[0] = 0x55;
	buff_send[1] = 0x55;
	
	/* frame type */
	buff_send[2] = 0x54;

	/* degree */
	buff_send[3] = (int)curent_degre / 256;
	buff_send[4] = (int)curent_degre % 256;

	/* shift */
	buff_send[5] = sum_shift / 256;
	buff_send[6] = sum_shift % 256;
	
	/* check sum */
	buff_send[7] = buff_send[3] + buff_send[4] + buff_send[5] + buff_send[6];

	/* farme end */
	buff_send[8] = 0xaa;
	buff_send[9] = 0xaa;
	
	/* append data to /mnt/car_log.txt */
//	FILE *fp_car_log;
//	fp_car_log = fopen("/mnt/car_log.txt", "a");
//	fprintf(fp_car_log, "degree:%d, shift:%d\n", buff_send[3] *256 + buff_send[4], buff_send[5] *256 + buff_send[6] );
//	fclose(fp_car_log);

	/* return */
	return send(fd_sockt_to, buff_send, 10, 0);

}
