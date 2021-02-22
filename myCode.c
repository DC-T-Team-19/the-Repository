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

#define PBSIZE 4096
#define SINESIZE 1024
#define PI 3.141592653589793

int16_t PlayBuff[PBSIZE];
int16_t SineBuff[SINESIZE];
int16_t modBuff[SINESIZE];
uint16_t buffer_offset = 0;
enum eNoteStatus { ready, going, finish }  noteStatus = ready;
enum eBufferStatus { empty, finished, firstHalfReq, firstHalfDone, secondHalfReq, secondHalfDone }  bufferStatus = empty;

// Set up the first note:
float noteFrequencyLeft = 660.0f;
float noteFrequencyRight = 661.0f;
float currentPhaseLeft = 0.0;
float currentPhaseRight = 0.0;
float phaseIncLeft = 0.0f;
float phaseIncRight = 0.0f;
float amplitude = 0.1;
float vibratoFreq = 330;

void mySysInitCode(void) {
	  initAudioTimer();
	  myAudioInitialisePeripherals(OUTPUT_DEVICE_AUTO, 80, AUDIO_FREQUENCY_44K);

	  // I'd also like to get the GPIOs set up for the LEDs so I can use them
	  // for debugging, so enable the clock to GPIOD and set the LED pins
	  // D12, D13, D14 and D15 to outputs:
	  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	  GPIOD->MODER |= 0x55 << 24;
}

void mySetupThingsStuff(void) {
	for (int i = 0; i <= SINESIZE; i++) {
		float q = amplitude * 32760 * sin(i * 2.0 * PI / SINESIZE);
		SineBuff[i] = (int16_t)q;
		float p = amplitude * 32760 * sin(i * 2.0 * PI / SINESIZE);
		modBuff[i] = (int16_t)p;////made another buff
	}
    for(int i=0; i <= PBSIZE; i++) { PlayBuff[i] = 0; } // Silence the buffer

    phaseIncLeft = SINESIZE * noteFrequencyLeft/ AUDIO_FREQUENCY_44K;
    phaseIncRight = SINESIZE * noteFrequencyRight/ AUDIO_FREQUENCY_44K;

	// Start the audio driver play routine:
	myAudioStartPlaying((int16_t *)&PlayBuff[0], PBSIZE);
	noteStatus = going;
}


//							V --- Max edit
void myMainWhileLoopStuff(float freq){ //float freq is the frequency that willl be played

	phaseIncLeft = SINESIZE * freq/ AUDIO_FREQUENCY_44K;
	phaseIncRight = SINESIZE * (freq +1) / AUDIO_FREQUENCY_44K;

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
/*
float modulator(int onOff, float freq, float intensity) {

	if(onOff = 1) {


	}
	else return 1.0;
}
*/
