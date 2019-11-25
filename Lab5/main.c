#include <lpc17xx.h>
#include <cmsis_os2.h>
#include <stdio.h>
#include "GLCD.h"
#include "random.h"

// GLOBAL VARIABLES: crosshair offset to make 'center' of LCD (0,0)
const int X_OFFSET = 150, Y_OFFSET = 120,HITBOX_SIZE = 10;
int aimX, aimY, shotsLeft; // convention: (0,0) is middle of screen
int targetX, targetY, prevTarX,prevTarY; // target moves across screen, must erase previous location
// tracking shots
int shotsHit = 0;
// graphics for crosshairs, red circle of target, and erasing their previous locations (black)
unsigned short whiteBar[] = {White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White,White};
unsigned short blackBar[] = {Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black};
unsigned short targetBMP[] = {Black,Black,Red,Red,Red,Black,Black,
														Black,Red,Red,Red,Red,Red,Black,
														Red,Red,Red,Red,Red,Red,Red,
														Red,Red,Red,Red,Red,Red,Red,
														Red,Red,Red,Red,Red,Red,Red,
														Black,Red,Red,Red,Red,Red,Black,
														Black,Black,Red,Red,Red,Black,Black};
int hit = 0;
int gameOver = 0;
	
void lcd(void *args) {
	// initialize LCD
	unsigned char scoreStr[] = "";
	unsigned char shotsRemain[] = "";
	GLCD_Init();
	GLCD_Clear(Black);
	
	while(1) {
		if (gameOver == 1) {
			GLCD_Clear(White);
			gameOver++; // move past this so game over screen doesn't keep updating - static end
		} else if (gameOver == 2) { // game over screen
			GLCD_DisplayString(3,5,1,(unsigned char *)"GAME OVER");
			GLCD_DisplayString(5,5,1,(unsigned char *)"YOUR SCORE:");
			sprintf(scoreStr,"%i",shotsHit);
			GLCD_DisplayString(6,9,1,scoreStr);
			osThreadExit();
		} else {
			// erase previous location of target
			GLCD_Bitmap(prevTarX-1,prevTarY-1,10,10,(unsigned char *)blackBar);
			if (!hit) { // draw target at new location if it's not yet hit
				GLCD_Bitmap(targetX,targetY,7,7,(unsigned char *)targetBMP);
			}
			// update location vars
			prevTarX = targetX;
			prevTarY = targetY;
			// draw new crosshair location, erase directly to its sides
			GLCD_Bitmap(aimX+X_OFFSET,0,1,240,(unsigned char *)whiteBar);
			GLCD_Bitmap(aimX+X_OFFSET+1,0,1,240,(unsigned char *)blackBar);
			GLCD_Bitmap(aimX+X_OFFSET-1,0,1,240,(unsigned char *)blackBar);
			GLCD_Bitmap(0,aimY+Y_OFFSET,300,1,(unsigned char *)whiteBar);
			GLCD_Bitmap(0,aimY+Y_OFFSET+1,300,1,(unsigned char *)blackBar);
			GLCD_Bitmap(0,aimY+Y_OFFSET-1,300,1,(unsigned char *)blackBar);
			// display to user how many shots they have left
			sprintf(shotsRemain,"%i",shotsLeft);
			GLCD_DisplayString(0,0,1,shotsRemain);
			osThreadYield();
		}
	}
}

void shots(void *args) {
	// enable pushbutton and LEDs
	LPC_GPIO1->FIODIR |= 0xB0000000;
	LPC_GPIO2->FIODIR |= 0x0000007C;
	while(1) {
		if(!(LPC_GPIO2->FIOPIN & 0x00000400)) { // if button pressed (user shoots)
			// depending on which shot was taken, update LEDs - turn on if corresponding shot hit
			if (shotsLeft == 8) { // top LED
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET>= targetY-HITBOX_SIZE) {
					LPC_GPIO1->FIOSET |= 0x10000000;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 7) {
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO1->FIOSET |= 0x20000000;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 6) {
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO1->FIOSET |= 0x80000000;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 5) {
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO2->FIOSET |= 0x00000004;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 4) {
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO2->FIOSET |= 0x00000008;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 3) {
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO2->FIOSET |= 0x00000010;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 2) {
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO2->FIOSET |= 0x00000020;
					hit = 1;
					shotsHit++;
				}
			} else if (shotsLeft == 1) { // bottom LED
				shotsLeft--;
				if (!hit && aimX + X_OFFSET <= targetX+HITBOX_SIZE && aimX + X_OFFSET >= targetX-HITBOX_SIZE && aimY + Y_OFFSET <= targetY+HITBOX_SIZE && aimY + Y_OFFSET >= targetY-HITBOX_SIZE) {
					LPC_GPIO2->FIOSET |= 0x00000040;
					hit = 1;
					shotsHit++;
				}
				gameOver = 1; // game ends when no shots remain to be taken
			}
			while(!(LPC_GPIO2->FIOPIN & 0x00000400));
		}
		
		// no longer need to update LEDs when game is over
		if (gameOver > 0)
			osThreadExit();
		
		osDelay(osKernelGetTickFreq()/50); // doesn't need to update in perfect sync with LCD
	}
}

void joystick(void *args) {
	// activate joystick pins 20, 23-26
	LPC_GPIO1->FIODIR &= ~0x079000000;
	
	// move crosshair centre co-ordinates depending on joystick position
	while(1) {
		if ((LPC_GPIO1->FIOPIN & 0x00800000) == 0)
			aimY--;
		else if ((LPC_GPIO1->FIOPIN & 0x04000000) == 0)
			aimX--;
		else if ((LPC_GPIO1->FIOPIN & 0x02000000) == 0)
			aimY++;
		else if ((LPC_GPIO1->FIOPIN & 0x01000000) == 0)
			aimX++;
		
		// crosshairs no longer move when game is over
		if (gameOver > 0)
			osThreadExit();
		
		osDelay(osKernelGetTickFreq()/100); // determines speed of crosshairs
	}
}

/* POSSIBLE IMPLEMENTATION INSTEAD OF JOYSTICK
void IMU(void *args) {
	// START IMU
	while(1) {
		
		osThreadYield();
	}
}
*/

void target(void *args) {
	while (1) {
		// re-initialize variables once the target goes offscreen
		hit = 0;
		targetX = 0;
		targetY = 0+(uint16_t)(lfsr113() >> 25); // randomize height
		uint16_t endX = 300+(uint16_t)(lfsr113() >> 25); // randomize how far offscreen it goes - therefore randomizes respawn
		uint16_t speedX = 0+(uint16_t)(lfsr113() >> 25); // randomize horizontal speed
		uint16_t speedY = (0+(uint16_t)(lfsr113() >> 25))/2; // randomize vertical speed
		
		int count = 0; // keeps track of when y will change (targets primarily 'fly' horizontally)
		for ( ; targetX < endX; targetX++) { // horizontal motion
			count++;
			if (count % speedY == 0) // count reaches multiple of speedY, change target height
				targetY++;
			osDelay(osKernelGetTickFreq()/speedX/2); // randomized speed of target
		}
		
		// no more targets when game ends
		if (gameOver > 0)
			osThreadExit();
	}
}

/* FUTURE IMPLEMENTATION OF TARGET TASK - ALLOWS RANDOMIZATION OF DIRECTION IN BOTH X AND Y (SOME SLOW TARGETS AT THE MOMENT ARE ANNOYING)
void target(void *args) {
	while (1) {
		int count = 0;
		hit = 0;
		int xdir = 0+(uint16_t)(lfsr113() >> 25);
		if (xdir < 32)
			xdir = 0; // left
		else
			xdir = 1; // right
		int ydir = 0+(uint16_t)(lfsr113() >> 25);
		if (ydir < 32)
			ydir = 0; // down
		else
			ydir = 1; // up
		
		targetY = 50+(uint16_t)(lfsr113() >> 25);
		uint16_t speedY = (0+(uint16_t)(lfsr113() >> 25))/2;
		
		if (xdir) {
			uint16_t endX = 300+(uint16_t)(lfsr113() >> 25);
			targetX = 0;
			uint16_t speedX = 0+(uint16_t)(lfsr113() >> 25);
			for ( ; targetX < endX; targetX++) {
				count++;
				if (count % speedY == 0) {
					if (ydir)
						targetY++;
					else
						targetY--;
				}
				osDelay(osKernelGetTickFreq()/speedX);
			}
		} else {
			uint16_t endX = -1*(uint16_t)(lfsr113() >> 25);
			targetX = 300;
			uint16_t speedX = 0+(uint16_t)(lfsr113() >> 25);
			for ( ; targetX > endX; targetX--) {
				count++;
				if (count % speedY == 0) {
					if (ydir)
						targetY++;
					else
						targetY--;
				}
				osDelay(osKernelGetTickFreq()/speedX);
			}
		}
		
		if (gameOver > 0)
			osThreadExit();
	}
}
*/

int main(void) {
	shotsLeft = 8;
	osKernelInitialize();
	osThreadNew(lcd,NULL,NULL);
	osThreadNew(joystick,NULL,NULL);
	osThreadNew(shots,NULL,NULL);
	osThreadNew(target,NULL,NULL);
	osKernelStart();
}
