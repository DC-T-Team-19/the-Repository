/*
 * interfacing.c
 *
 *  Created on: 23 Feb 2021
 *      Author: Work
 */
#include "interfacing.h"

GPIO_InitTypeDef pinConstructor(uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed){
	GPIO_InitTypeDef pinStruct;
	pinStruct.Pin = pin;
	pinStruct.Mode = mode;
	pinStruct.Pull = pull;
	pinStruct.Speed = speed;
	return pinStruct;
}

int checkButtonPress(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef q){
	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOx, q.Pin);
	if(pinState == 1){
		return 1;
	}else{
		return 0;
	}
};



