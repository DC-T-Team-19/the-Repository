/*
 * clockTime.c
 *
 *  Created on: 28 Feb 2021
 *      Author: joshm + davidm
 */

#include "clockTime.h"
#include <math.h>

// output pin PC9 oscillates at one quarter of the frequency of the system clock
void setupClockPin() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER &= 0xFFF3FFFF;
    GPIOC->MODER |= 0x00080000;
    GPIOC->OTYPER &= 0xFFFFFDFF;
    GPIOC->OSPEEDR |= 0x000C0000; // set speed to fast for bit 9
    GPIOC->PUPDR &= 0xFFF3FFFF;
    GPIOC->AFR[1] &= 0xFFFFFF0F;
    RCC->CFGR &= 0x07FFFFFF;
    RCC->CFGR |= 0x30000000; // pre-scaler to divide by 4
    __GPIOC_CLK_ENABLE();
}


//reads from the PC9 pin for the clock
int readClockPin(){
	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);
	if(pinState == 1){
		return 1;
	}
	else{
		return 0;
	}
}

//function to check how many times the clock goes high
/*void clockTest(int highAmount) {
	setupClockPin();

	while (highAmount != 0) {

		if (statPC9 == 1) {
			highAmount --;
		}

	}
	changeFrequency(440,440);
}*/
