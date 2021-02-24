/*
 * interfacing.c
 *
 *  Created on: 23 Feb 2021
 *      Author: Work
 */
#include "interfacing.h"


//Ena
void initialiseButton(GPIO_InitTypeDef q){
	HAL_GPIO_Init(GPIOA, &q);
};

int checkButtonPress(GPIO_InitTypeDef q){
	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOA, q.Pin);
	if(pinState == 1){
		return 1;
	}else{
		return 0;
	}
};

