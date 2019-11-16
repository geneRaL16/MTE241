#include <lpc17xx.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include <stdbool.h>
#include "GLCD.h"


void LEDS(void) {
	// initialize LEDs
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIODIR |= 0x0000007C;
	
	// 123 base 10 = 01111011 base 2, display on LEDs. MSB is P1.28
	LPC_GPIO1->FIOSET |= 0xA0000000;
	LPC_GPIO2->FIOSET |= 0x0000006C;
}

void Joystick(void) {
	// activate joystick pins 20, 23-26
	LPC_GPIO1->FIODIR &= ~0x079000000; // should it be activated in the other direction?
	printf("Joystick on\n");
	char* dir = "";
	char* pressed = "";
	
	while(1) {
		if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0)
			dir = "DOWN";
		if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0)
			dir = "RIGHT";
		if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0)
			dir = "UP";
		if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0)
			dir = "LEFT";
		
		if (LPC_GPIO1->FIOPIN & 0x00100000)
			pressed = "not pressed";
		else
			pressed = "pressed";
		
		printf("%s, %s\n", dir, pressed);
	}
}

void Potentiometer(void) {	
	// allow analog reading
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	LPC_PINCON->PINSEL1 |= (0X01<<18);
	
	LPC_SC->PCONP |= (0x00001000);
	
	LPC_ADC->ADCR = (1 << 2) |
									(4 << 8) | // 25MHz/5
									(1 << 21);
	
	while(1) {
		LPC_ADC->ADCR |= 0x01 << 24;
		while(!(LPC_ADC->ADGDR & 0x80000000));
		printf("%d\n",((LPC_ADC->ADGDR & 0x0000FFF0) >> 4));
	}
}

void LCD(void) {
	GLCD_Init();
	GLCD_Clear(Blue);
	GLCD_SetBackColor(Blue);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(4,3,1,"Hello, world!");
}

void pushbutton2(void *arg){
	LPC_GPIO2->FIODIR |= 0x0000007C; // enable pushbutton and LEDs
	LPC_GPIO2->FIOPIN &= ~0x0000007C;
	while(1) {
		if(!(LPC_GPIO2->FIOPIN & 0x00000400)) {
			if (LPC_GPIO2->FIOSET & 0x00000040)
				LPC_GPIO2->FIOCLR |= 0x00000040;
			else
				LPC_GPIO2->FIOSET |= 0x00000040;
			while(!(LPC_GPIO2->FIOPIN & 0x00000400));
		}
		osDelay(100);
	}
}

void potent2(void *arg){
	// allow analog reading
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	LPC_PINCON->PINSEL1 |= (0X01<<18);
	
	LPC_SC->PCONP |= (0x00001000);
	
	LPC_ADC->ADCR = (1 << 2) |
									(4 << 8) | // 25MHz/5
									(1 << 21);
	
	while(1) {
		LPC_ADC->ADCR |= 0x01 << 24;
		while(!(LPC_ADC->ADGDR & 0x80000000));
		printf("%d\n",((LPC_ADC->ADGDR & 0x0000FFF0) >> 4));
		osDelay(100);
	}
}

void joystick2(void *arg){
	// activate joystick pins 20, 23-26
	LPC_GPIO1->FIODIR &= ~0x079000000; // should it be activated in the other direction?
	GLCD_Init();
	unsigned char* dir = "";
	unsigned char* pressed = "";
	
	while(1) {
		if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0)
			dir = "DOWN";
		else if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0)
			dir = "RIGHT";
		else if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0)
			dir = "UP";
		else if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0)
			dir = "LEFT";
		else
			dir = "no direction";
		
		if (LPC_GPIO1->FIOPIN & 0x00100000)
			pressed = "not pressed";
		else
			pressed = "pressed";
		
		
		GLCD_Clear(Blue);
		GLCD_SetBackColor(Blue);
		GLCD_SetTextColor(White);
		GLCD_DisplayString(4,3,1,dir);
		GLCD_DisplayString(5,3,1,pressed);
		osDelay(100);
	}
}

int main(void) {
	//LEDS();
	//Joystick();
	//Potentiometer();
	//LCD();
	
	osKernelInitialize();
	osThreadNew(pushbutton2, NULL, NULL);
	osThreadNew(joystick2, NULL, NULL);
	osThreadNew(potent2, NULL, NULL);
	printf("%d",0);
	osKernelStart();
	
	while(1);
}
