/*
 * interfacing.h
 *
 *  Created on: 23 Feb 2021
 *      Author: Work
 */

#ifndef INC_INTERFACING_H_
#define INC_INTERFACING_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

//Use GPIOA for all inputs

void initialiseButton(GPIO_InitTypeDef q);
int checkButtonPress(GPIO_InitTypeDef q);


#endif /* INC_INTERFACING_H_ */
