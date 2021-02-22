/*
 * myCode.h
 *
 *  This is the header file with the includes and function declarations for
 *  the STM32CubeIDE getting started with audio project for the STM32F407
 *  Discovery board.  It should be included in the "main.c" file.
 */

#ifndef INC_MYCODE_H_
#define INC_MYCODE_H_

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

void mySysInitCode(void);
void mySetupThingsStuff(void);
void myMainWhileLoopStuff(float);
float modulateBy(uint16_t, float);

#endif /* INC_MYCODE_H_ */
