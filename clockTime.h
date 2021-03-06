  /*
 * clockTime.h
 *
 *  Created on: 28 Feb 2021
 *      Author: joshm + davidm
 */

#ifndef INC_CLOCKTIME_H_
#define INC_CLOCKTIME_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"

#include "Audio_Drivers.h"
#include "audio_replacement.h"
#include "basicFunctions.h"

//Clock
void setupClockPin();
void clockTest(int highAmount);
int readClockPin();
void checkCount(int amount);
void externalClock();
void speedUpClock();

//Interupts
void enableInterupts();
void setPriority(int value);

//Peripherals (Chapter 11)
void clockCountEn();
void TIM3_IRGHandler();
void asynCallback();

#endif /* INC_CLOCKTIME_H_ */
