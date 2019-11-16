#include <lpc17xx.h>
#include <cmsis_os2.h>
#include <stdio.h>
#include "GLCD.h"

// MUTEXES
// GLOBAL VARIABLES (E.G. SCORE (BOOL ARRAY OF SHOTS HIT?), BOOLEAN GAMEOVER, INT SHOTS REMAINING)

void lcd(void *args) {
	GLCD_Init();
	while(1) {
		
		osThreadYield();
	}
}

void button(void *args) {
	// START BUTTON
	while(1) {
		// IMPLEMENT INTERRUPT
		// DEBOUNCE
		osThreadYield();
	}
}

void leds(void *args) {
	// START LEDS
	while(1) {
		// IMPLEMENT OUTPUT
		osThreadYield();
	}
}

void joystick(void *args) {
	// START JOYSTICK
	while(1) {
		
		osThreadYield();
	}
}

void IMU(void *args) {
	// START IMU
	while(1) {
		
		osThreadYield();
	}
}

int main(void) {
	osKernelInitialize();
	osThreadNew(lcd,NULL,NULL);
	osThreadNew(button,NULL,NULL);
	osThreadNew(leds,NULL,NULL);
	osThreadNew(/*pick IMU or joystick*/,NULL,NULL);
	osKernelStart();
	// TODO: EXIT ROUTINE
}