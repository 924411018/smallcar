car:car.c opencompass.c opencom.c setcom.c recorrect_compass.c getdegree.c server_setup.c
	arm-linux-gcc car.c opencompass.c opencom.c setcom.c recorrect_compass.c getdegree.c server_setup.c analysis_buff_recv.c report_status.c -o car_v3
clean:
	rm car_v3
