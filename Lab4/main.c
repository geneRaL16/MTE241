#include <lpc17xx.h>
#include <cmsis_os2.h>
#include <stdio.h>
#include "random.h"
#include "GLCD.h"

// initializing message queues and mutexs
osMessageQueueId_t q_id[2];

osMutexId_t clientMutex;
osMutexId_t serverMutex[2];

//defining frequencies rates
int const MUTEX_FREQ = 100;
int const ARRIVAL_RATE = 18;
int const SERVICE_RATE = 10;

//defining the sent, received, and overflow counters
int sent[2] = {0, 0};
int received[2] = {0,0};
int overflow[2] = {0, 0};

void monitor(void *args) {
	// initializes and populates the lcd screen with titles
	GLCD_Init();
	GLCD_SetBackColor(Blue);
	GLCD_SetTextColor(White);
	unsigned char q1[] = "q1";
	unsigned char q2[] = "q2";
	unsigned char s[] = "sent";
	unsigned char recv[] = "recv";
	unsigned char over[] = "over";
	unsigned char t[] = "time";
	unsigned char sent1[] = "a";
	unsigned char sent2[] = "b";
	unsigned char over1[] = "c";
	unsigned char over2[] = "d";
	unsigned char reced1[] = "e";
	unsigned char reced2[] = "f";
	unsigned char clk[] = "";
	int runClock = 0;
	
	while(1) {
		//places the titles on the lcd screen
		GLCD_Clear(Blue);
		GLCD_DisplayString(0,6,1,q1);
		GLCD_DisplayString(0,14,1,q2);
		GLCD_DisplayString(2,0,1,s);
		GLCD_DisplayString(4,0,1,recv);
		GLCD_DisplayString(6,0,1,over);
		GLCD_DisplayString(8,0,1,t);
		
		//displays the LCD infomration for sent and overflow in for queue 1 and 2
		osMutexAcquire(clientMutex,osKernelGetTickFreq()/MUTEX_FREQ);
			sprintf(sent1,"%i",sent[0]);
			GLCD_DisplayString(2,6,1,sent1);
			sprintf(sent2,"%i",sent[1]);
			GLCD_DisplayString(2,14,1,sent2);
			sprintf(over1,"%i",overflow[0]);
			GLCD_DisplayString(4,6,1,over1);
			sprintf(over2,"%i",overflow[1]);
			GLCD_DisplayString(4,14,1,over2);
		osMutexRelease(clientMutex);
		
		//queue 1 receieved display
		osMutexAcquire(serverMutex[0],osKernelGetTickFreq()/MUTEX_FREQ);
			sprintf(reced1,"%i",received[0]);
			GLCD_DisplayString(6,6,1,reced1);
		osMutexRelease(serverMutex[0]);
		
		//queue 2 receieved display
		osMutexAcquire(serverMutex[1],osKernelGetTickFreq()/MUTEX_FREQ);
			sprintf(reced2,"%i",received[1]);
			GLCD_DisplayString(6,14,1,reced2);
		osMutexRelease(serverMutex[1]);
		
		//displaying runtime
		sprintf(clk,"%i",runClock++);
		GLCD_DisplayString(8,9,1,clk);
		
		osDelay(osKernelGetTickFreq());
	}
}

void client(void *args) {
	int msg;
	while(1) {
		//aquires mutex because queue information will change
		osMutexAcquire(clientMutex,osKernelGetTickFreq()/MUTEX_FREQ);
			for (int i = 0; i < 2; i++) {
				osMutexAcquire(serverMutex[i],osKernelGetTickFreq()/MUTEX_FREQ);
				//checks forr resource error with put message
				if (osMessageQueuePut(q_id[i], &msg, 0, 0) == osErrorResource)
					overflow[i]++; // increments error count
				osMutexRelease(serverMutex[i]); // releases mutex
				sent[i]++;
			}
		osMutexRelease(clientMutex);
		osDelay(((next_event() * osKernelGetTickFreq())/ARRIVAL_RATE) >> 16); //delays for randomized next event
	}
}

void server(void *queue) { // queue passed by reference slects the server 
	int msg;
	
	while(1) {
		//aquires mutex for server
		osMutexAcquire(serverMutex[*((int *) queue)],osKernelGetTickFreq()/MUTEX_FREQ);
			osMessageQueueGet(q_id[*((int *) queue)], &msg, NULL, osWaitForever); // receives msg
		received[*((int *) queue)]++;
		osMutexRelease(serverMutex[*((int *) queue)]);
		osDelay(((next_event() * osKernelGetTickFreq())/SERVICE_RATE) >> 16); //delays for randomized next event
	}
}

int main(void) {
	
	int zero = 0;
	int one = 1;
	
	//initializes kernel, queues, and threads
	osKernelInitialize();
	q_id[0] = osMessageQueueNew(10, sizeof(int), NULL);
	q_id[1] = osMessageQueueNew(10, sizeof(int), NULL);
	osThreadNew(monitor, NULL, NULL);
	osThreadNew(client, NULL, NULL);
	osThreadNew(server, &zero, NULL);
	osThreadNew(server, &one, NULL);
	osKernelStart(); // starts threads
	
	while(1);
}
