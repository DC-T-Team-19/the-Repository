
#include "basicFunctions.h"
#include <math.h>

#define PBSIZE 4096
#define SINESIZE 1024
#define PI 3.141592653589793

int16_t PlayBuff[PBSIZE];
int16_t SineBuff[SINESIZE];
float SineModBuff[SINESIZE];
uint16_t buffer_offset = 0;
enum eNoteStatus { ready, going, finish }  noteStatus = ready;
enum eBufferStatus { empty, finished, firstHalfReq, firstHalfDone, secondHalfReq, secondHalfDone }  bufferStatus = empty;
enum efreqModStatus { on, off } freqModStatus = on;

// Set up the first note:
float noteFrequencyLeft = 220.0f;
float noteFrequencyRight = 220.0f;
float noteFrequencyMod = 1.0f;
float currentPhaseLeft = 0.0;
float currentPhaseRight = 0.0;
float currentPhaseMod = 0.0f;
float phaseIncLeft = 0.0f;
float phaseIncRight = 0.0f;
float phaseIncMod = 0.0f;
float ampMod = 1;
uint32_t maxAmplitude = 32760;
int range = 1;

void initialiseAudio(void) {
	  initAudioTimer();
	  myAudioInitialisePeripherals(OUTPUT_DEVICE_AUTO, 80, AUDIO_FREQUENCY_44K);

	  // I'd also like to get the GPIOs set up for the LEDs so I can use them
	  // for debugging, so enable the clock to GPIOD and set the LED pins
	  // D12, D13, D14 and D15 to outputs:
	  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	  GPIOD->MODER |= 0x55 << 24;
}

void setupAudio(){
	createSineTable();
	createModSineTable();
    silenceBuff(); // Silence the buffer

    updatePhase();

	// Start the audio driver play routine:
	start();
}

void mainWhileLoop(){
	if (bufferStatus == firstHalfReq) {
		firstHalfReqFill();
	}
	else if (bufferStatus == secondHalfReq) {
		secondHalfReqFill();
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

void changeFrequency(uint32_t newFreq){
	if(newFreq != noteFrequencyLeft){
		noteFrequencyLeft = newFreq;
		noteFrequencyRight = newFreq;
	}
    //updatePhase();
}

void changeFrequencyMod(float newFreq){
	if(freqModStatus == on){
		noteFrequencyMod = newFreq;
	}
	else{
		noteFrequencyMod = 0;
	}
	//phaseIncMod = SINESIZE * noteFrequencyMod / AUDIO_FREQUENCY_44K;
}

int checkFreqModActive(){
	if(freqModStatus == on){
		return 1;
	}
	else{
		return 0;
	}
}

void setFreqModState(){
	if(freqModStatus == on){
		freqModStatus = off;
		noteFrequencyMod = 0;
	}
	else if(freqModStatus = off){
		freqModStatus = on;
	}
}

void updatePhase(){
    phaseIncLeft = SINESIZE * noteFrequencyLeft / AUDIO_FREQUENCY_44K;
    phaseIncRight = SINESIZE * noteFrequencyRight / AUDIO_FREQUENCY_44K;
}

void changeAmplitude(uint32_t value){
	ampMod = value/4096;
}

void firstHalfReqFill(){
	uint32_t startFill = 0, endFill = 0;
	endFill = PBSIZE/2;
	bufferStatus = firstHalfDone;

	fill(startFill, endFill);
}

void secondHalfReqFill(){
	uint32_t startFill = 0, endFill = 0;
	startFill = PBSIZE/2;
	endFill = PBSIZE;
	bufferStatus = secondHalfDone;

	fill(startFill, endFill);
}

void silenceBuff(){
	for(int i=0; i <= PBSIZE; i++) { PlayBuff[i] = 0; } // Silence the buffer
}

void stop(){
	noteStatus = finish;
}

void start(){
	myAudioStartPlaying((int16_t *)&PlayBuff[0], PBSIZE);
	noteStatus = going;
}

void createSineTable(){
	for (int i = 0; i <= SINESIZE; i++) {
		float q = maxAmplitude * sin(i * 2.0 * PI / SINESIZE);
		SineBuff[i] = (int16_t)q;
	}
}

void createModSineTable(){
	for (int i = 0; i <= SINESIZE; i++) {
		float q = sin(i * 2.0 * PI / SINESIZE);
		SineModBuff[i] = q;
	}
}

void fill(uint32_t startFill, uint32_t endFill){
	if(startFill != endFill){
		for (int i = startFill; i < endFill; i += 2){
			phaseIncMod = SINESIZE * noteFrequencyMod/ AUDIO_FREQUENCY_44K;
			currentPhaseMod += phaseIncMod;
			if (currentPhaseMod > SINESIZE) currentPhaseMod -= SINESIZE;

			phaseIncLeft = SINESIZE * noteFrequencyLeft * powf(range+1, SineModBuff[(uint16_t)currentPhaseMod])/ AUDIO_FREQUENCY_44K;
			currentPhaseLeft += phaseIncLeft;
			if (currentPhaseLeft > SINESIZE) currentPhaseLeft -= SINESIZE;
			int32_t nextSampleLeft = SineBuff[(uint16_t)(currentPhaseLeft)];

			phaseIncRight = SINESIZE * noteFrequencyRight * powf(range+1, SineModBuff[(uint16_t)currentPhaseMod])/ AUDIO_FREQUENCY_44K;
			currentPhaseRight += phaseIncRight;
			if (currentPhaseRight > SINESIZE) currentPhaseRight -= SINESIZE;
			int32_t nextSampleRight = SineBuff[(uint16_t)(currentPhaseRight)];

			PlayBuff[i] = (uint16_t)(nextSampleLeft);
			PlayBuff[i + 1] = (uint16_t)(nextSampleRight);
		}
	}
}


