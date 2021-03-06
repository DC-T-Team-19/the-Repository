/*
 * basicFunctions.h
 *
 *  Created on: 16 Feb 2021
 *      Author: shobe
 */

#ifndef INC_BASICFUNCTIONS_H_
#define INC_BASICFUNCTIONS_H_

#include "Audio_Drivers.h"
#include "audio_replacement.h"

// Some user-friendly ways to turn the LEDS on and off:
#define BLUEON GPIOD->ODR |= 0x8000
#define BLUEOFF GPIOD->ODR &= ~0x8000
#define REDON GPIOD->ODR |= 0x4000
#define REDOFF GPIOD->ODR &= ~0x4000
#define ORANGEON GPIOD->ODR |= 0x2000
#define ORANGEOFF GPIOD->ODR &= ~0x2000
#define GREENON GPIOD->ODR |= 0x1000
#define GREENOFF GPIOD->ODR &= ~0x1000

void initialiseAudio(void);
void setupAudio(void);
void mainWhileLoop(void);
void changeAmplitude(uint32_t newAmplitude);
void firstHalfReqFill(void);
void secondHalfReqFill(void);
void fill(uint32_t startFill, uint32_t endFill);
void changeFrequency(float newLeftFreq, float newRightFreq);
void updatePhase();
void silenceBuff();
void stop();
void start();
void createSineTable();


#endif /* INC_BASICFUNCTIONS_H_ */
