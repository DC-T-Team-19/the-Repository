/*
 * clockTime.c
 *
 *  Created on: 28 Feb 2021
 *      Author: joshm + davidm
 */

#include "clockTime.h"
#include <math.h>
#include <stdio.h>

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

//check how many times the clock pin goes high
void checkCount(int amount) {
	while (amount != 0) {
		if (readClockPin() == 1) {
			amount --;
		}
	}

	//changeFrequency(320, 320);
}

//To use the external clock, the clock has to be enabled in the reset and clock control (RCC) control
//register (RCC_CR), and then selected as the main clock source in the RCC configuration register
//(RCC_CFGR).
void externalClock() {
	RCC->CR |= RCC_CR_HSEON; // Enable the HSE clock
	while(!(RCC->CR & RCC_CR_HSERDY)){} // Wait for clock to be ready
	RCC->CFGR &= ~RCC_CFGR_SW_Msk; // Select HSE as the new
	RCC->CFGR |= RCC_CFGR_SW_HSE; // system clock
}

//speeds up the clock from 8MHz to 24MHz
void speedUpClock() {
	RCC->CR |= RCC_CR_HSEON; // Enable the HSE clock
	while (!(RCC->CR & RCC_CR_HSERDY)){} // Wait for HSE clock
	RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE // Use HSE as source
	 | (5 << 0) // Set M = 5
	 | (60 << 6) // Set N = 60
	 | (1 << 16) // Set P = 4
	 | (2 << 24); // Set Q = 2
	//Having set up the PLL, it needs to be enabled, and then you have to wait for it to settle down and
	//acquire lock:
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){}
	//the system clock can be configured to use the faster PLL clock frequency:
	RCC->CFGR &= ~RCC_CFGR_SW_Msk; // Set the clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL; // from the PLL
	while(!(RCC->CFGR & RCC_CFGR_SW_PLL));
}

