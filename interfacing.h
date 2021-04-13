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
#include "stm32f4xx_hal_dma.h"

//Use GPIOA for all inputs
int checkButtonPress(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef q);
GPIO_InitTypeDef pinConstructor(uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed);

void enableDMA(uint32_t DMABuffer1);

#endif /* INC_INTERFACING_H_ */
