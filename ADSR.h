/*
 * ADSR.h
 *
 *  Created on: 10 Mar 2021
 *      Author: JoshM, DavidM, MaxB
 */

#ifndef INC_ADSR_H_
#define INC_ADSR_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"

#include "Audio_Drivers.h"
#include "audio_replacement.h"
#include "basicFunctions.h"

float* createATable();
void createDTable(float constantSustain);
void createRTable(float constantSustain);
void changeADSR();

#endif /* INC_ADSR_H_ */
