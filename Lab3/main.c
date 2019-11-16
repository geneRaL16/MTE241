#include <lpc17xx.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include "MPU9250.h"
#include "led.h"
#include "sensor_fusion.h"

const float FREQ = 256; //sets the global variable to a value we specify, easier way to change value

// Mutexes are shared between threads
osMutexId_t IMUVals;
osMutexId_t PYR;

void readIMU(void *arg) { // reads 9 IMU sensor vals
	while(1) {
		osMutexAcquire(IMUVals,10);
		// read from IMU, store in relevant variables per the function
		MPU9250_read_gyro();
		MPU9250_read_acc();
		MPU9250_read_mag();
		osMutexRelease(IMUVals); // IMU data ready
		
		osThreadYield();
	}
}

void sensorFusion(void *arg) { // computes 3 rot'nal output vals from 9 sensor vals
	while(1) {
		osMutexAcquire(IMUVals,10); // IMU data ready
		// using the sensor_fusion algorithm to calculate pitch, yaw, roll based on
		//		gyroscope, accelerometer, magnetoscope readings in MPU9250.c
		if ((MPU9250_st_value & 0x00000010) == 16) { // if the magnetoscope reading is valid
			osMutexAcquire(PYR,10);
			sensor_fusion_update(MPU9250_gyro_data[0],MPU9250_gyro_data[1],MPU9250_gyro_data[2],
					MPU9250_accel_data[0],MPU9250_accel_data[1],MPU9250_accel_data[2],
					MPU9250_mag_data[0],MPU9250_mag_data[1],MPU9250_mag_data[2]);
			osMutexRelease(PYR); // rot'nal data calculated
		}
		osMutexRelease(IMUVals);
		
		osThreadYield();
	}
}

void sendData(void *arg) { // sends 3 rot'nal vals to computer
	while(1) {
		osMutexAcquire(PYR,10);
		// obtain pitch, yaw, roll data from the sensor_fusion algorithm
		float pitch = -sensor_fusion_getPitch();
		float yaw = -sensor_fusion_getYaw();
		float roll = -sensor_fusion_getRoll();
		osMutexRelease(PYR);
		
		printf("%f,%f,%f\n",yaw,pitch,roll); // send rot'nal to computer
		
		osThreadYield();
	}
}

int main(void) {
	//initial calibration of the MPU9250
	MPU9250_init(1,1);
	// initializes the LEDs
	LED_setup();
	//checks the communication with MPU by displays the value 0x71
	LED_display(MPU9250_whoami());
	
	//initializing the sensor fusion algorithm
	sensor_fusion_init();
	sensor_fusion_begin(FREQ);
	
	//create mutexes
	IMUVals = osMutexNew(NULL);
	PYR = osMutexNew(NULL);
	
	//start threads and run
	osKernelInitialize();
	osThreadNew(readIMU,NULL,NULL);
	osThreadNew(sensorFusion,NULL,NULL);
	osThreadNew(sendData,NULL,NULL);
	printf("%d\n",0);
	osKernelStart();
	
	while(1);
}
