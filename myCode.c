 /*
 * myCode.c
 *
 *  This is the file where I keep my application code to make it easier to read,
 *  since it's no longer hidden amongst all the automatically generated code in
 *  main.c.  It's also less likely to get lost if CubeMX reconfigures the uC
 *  and over-writes everything in main.c.
 */

#include "myCode.h"
#include <math.h>

// defining important variables
#define PBSIZE 4096 //size of buffer
#define SINESIZE 1024
#define PI 3.141592653589793

int16_t PlayBuff[PBSIZE];
int16_t SineBuff[SINESIZE];
uint16_t buffer_offset = 0;

//States of buffer and how to fill it up
enum eNoteStatus { ready, going, finish }  noteStatus = ready;
enum eBufferStatus { empty, finished, firstHalfReq, firstHalfDone, secondHalfReq, secondHalfDone }  bufferStatus = empty;

// Set up the first note:
float noteFrequencyLeft = 330.0f; //plays an E
float noteFrequencyRight = 331.0f;
float currentPhaseLeft = 0.0;
float currentPhaseRight = 0.0;
float phaseIncL = 0.0f;
float phaseIncR = 0.0f;
float phaseIncLeft = 0.0f;
float phaseIncRight = 0.0f;
float stepChange = 0.5; //change this value to change the frequency of the note

void mySysInitCode(void) {
	  initAudioTimer();
	  myAudioInitialisePeripherals(OUTPUT_DEVICE_AUTO, 80, AUDIO_FREQUENCY_44K);

	  // I'd also like to get the GPIOs set up for the LEDs so I can use them
	  // for debugging, so enable the clock to GPIOD and set the LED pins
	  // D12, D13, D14 and D15 to outputs:
	  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	  GPIOD->MODER |= 0x55 << 24;
}

float changeFrequencyL(float step) {
	phaseIncLeft = phaseIncL * step;
	return phaseIncLeft;
}

float changeFrequencyR(float step) {
	phaseIncRight = phaseIncR * step;
	return phaseIncRight;
}

void mySetupThingsStuff(void) {
	for (int i = 0; i <= SINESIZE; i++) { //loops until i is the size of the wavetable 1024
		float q = 32760 * sin(i * 2.0 * PI / SINESIZE);//32760 is the amplitude
		SineBuff[i] = (int16_t)q;
	}
    for(int i=0; i <= PBSIZE; i++) {
    	PlayBuff[i] = 0;
    } // Silence the buffer

    //Calculating the phase of stereo output
    phaseIncL = SINESIZE * noteFrequencyLeft / AUDIO_FREQUENCY_44K;
    phaseIncR = SINESIZE * noteFrequencyRight / AUDIO_FREQUENCY_44K;

    phaseIncLeft = changeFrequencyL(stepChange);
    phaseIncRight = changeFrequencyR(stepChange);

	// Start the audio driver play routine:
	myAudioStartPlaying((int16_t *)&PlayBuff[0], PBSIZE);
	noteStatus = going;
}

void myMainWhileLoopStuff(void){

	// If there's an active request to fill half of the buffer, then do so:
	uint32_t startFill = 0, endFill = 0;
	if (bufferStatus == firstHalfReq) {
		startFill = 0;
		endFill = PBSIZE / 2;
		bufferStatus = firstHalfDone;
	}
	else if (bufferStatus == secondHalfReq) {
		startFill = PBSIZE / 2;
		endFill = PBSIZE;
		bufferStatus = secondHalfDone;
	}
	if (startFill != endFill) {
		for (int i = startFill; i < endFill; i += 2) {

			currentPhaseLeft += phaseIncLeft;
			if (currentPhaseLeft > SINESIZE) currentPhaseLeft -= SINESIZE;
			int32_t nextSampleLeft = SineBuff[(uint16_t)(currentPhaseLeft)];

			currentPhaseRight += phaseIncRight;
			if (currentPhaseRight > SINESIZE) currentPhaseRight -= SINESIZE;
			int32_t nextSampleRight = SineBuff[(uint16_t)(currentPhaseRight)];

			PlayBuff[i] = (uint16_t)nextSampleLeft;
			PlayBuff[i + 1] = (uint16_t)nextSampleRight;
		}
	}
}

uint32_t debugLED = 0;
void myAudioHalfTransferCallback(void) {
	if (noteStatus == going) bufferStatus = firstHalfReq;
	if (debugLED > 25) {
		ORANGEOFF;
	}
	else if (debugLED > 5) {
		ORANGEON;
	}
}
void myAudioTransferCompleteCallback(void) {
	if (noteStatus == going) bufferStatus = secondHalfReq;
	myAudioChangeBuffer((int16_t *)PlayBuff, PBSIZE);
	if (debugLED++ >= 40) debugLED = 0;
	if (debugLED > 35) {
		REDOFF;
	}
	else if (debugLED > 15) {
		REDON;
	}
}
