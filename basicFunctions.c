
#include "basicFunctions.h"
#include <math.h>
#include "cs43l22.h"

#define PBSIZE 4096
#define SINESIZE 1024
#define PI 3.141592653589793
#define DAC_ADDR_ON_I2C 0x94

int16_t PlayBuff[PBSIZE];
int16_t SineBuff[SINESIZE];
float SineModBuff[SINESIZE];
uint16_t buffer_offset = 0;
enum eNoteStatus { ready, going, finish }  noteStatus = ready;
enum eBufferStatus { empty, finished, firstHalfReq, firstHalfDone, secondHalfReq, secondHalfDone }  bufferStatus = empty;
uint32_t debugLED = 0;

// Set up the first note:
float noteFrequencyLeft = 130.0f;
float noteFrequencyRight = 130.0f;
float noteFrequencyMod = 1.0f;
float currentPhaseLeft = 0.0;
float currentPhaseRight = 0.0;
float currentPhaseMod = 0.0f;
float phaseIncLeft = 0.0f;
float phaseIncRight = 0.0f;
float phaseIncMod = 0.0f;
float ampMod = 0.5f;
float leftPan = 1.0f;
float rightPan = 1.0f;
uint32_t maxAmplitude = 32760;
int range = 1;

//Initialise necessary peripherals
void initialiseAudio(void) {
	  initAudioTimer();
	  myAudioInitialisePeripherals(OUTPUT_DEVICE_AUTO, 80, AUDIO_FREQUENCY_44K);

	  // I'd also like to get the GPIOs set up for the LEDs so I can use them
	  // for debugging, so enable the clock to GPIOD and set the LED pins
	  // D12, D13, D14 and D15 to outputs:
	  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	  GPIOD->MODER |= 0x55 << 24;
}

//Set audio running
void setupAudio(){
	createSineTable();
	createModSineTable();
    silenceBuff(); // Silence the buffer

	phaseIncLeft = SINESIZE * noteFrequencyLeft * powf(range+1, SineModBuff[(uint16_t)currentPhaseMod])/ AUDIO_FREQUENCY_44K;
	phaseIncRight = SINESIZE * noteFrequencyRight * powf(range+1, SineModBuff[(uint16_t)currentPhaseMod])/ AUDIO_FREQUENCY_44K;

	// Start the audio driver play routine:
	start();
}

//Main loop for filling the buffer
void mainWhileLoop(){
	if (bufferStatus == firstHalfReq) {
		firstHalfReqFill();
	}
	else if (bufferStatus == secondHalfReq) {
		secondHalfReqFill();
	}
}

//Call when buffer half full
void myAudioHalfTransferCallback(void) {
	if (noteStatus == going) bufferStatus = firstHalfReq;
	if (debugLED > 25) {
		ORANGEOFF;
	}
	else if (debugLED > 5) {
		ORANGEON;
	}
}

//Call when buffer full
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

//Request to fill first half
void firstHalfReqFill(){
	uint32_t startFill = 0, endFill = 0;
	endFill = PBSIZE/2;
	bufferStatus = firstHalfDone;

	fill(startFill, endFill);
}

//Request to fill second half
void secondHalfReqFill(){
	uint32_t startFill = 0, endFill = 0;
	startFill = PBSIZE/2;
	endFill = PBSIZE;
	bufferStatus = secondHalfDone;

	fill(startFill, endFill);
}

// Silence the buffer
void silenceBuff(){
	for(int i=0; i <= PBSIZE; i++) { PlayBuff[i] = 0; }
}

//Stop the synthesis
void stop(){
	noteStatus = finish;
}

//Start the synthesis
void start(){
	myAudioStartPlaying((int16_t *)&PlayBuff[0], PBSIZE);
	noteStatus = going;
}

//Create the Sine table
void createSineTable(){
	for (int i = 0; i <= SINESIZE; i++) {
		float q = maxAmplitude * sin(i * 2.0 * PI / SINESIZE);
		SineBuff[i] = (int16_t)q;
	}
}

//Create the Sine table for the modulating frequency
void createModSineTable(){
	for (int i = 0; i <= SINESIZE; i++) {
		float q = sin(i * 2.0 * PI / SINESIZE);
		SineModBuff[i] = q;
	}
}


//Fill first or second half of buffer (determined by startFill and endFill)
void fill(uint32_t startFill, uint32_t endFill){
	int count = 0;
	if(startFill != endFill){
		for (int i = startFill; i < endFill; i += 2){
			//Modulating Frequency
			phaseIncMod = SINESIZE * noteFrequencyMod/ AUDIO_FREQUENCY_44K;
			currentPhaseMod += phaseIncMod;
			if (currentPhaseMod > SINESIZE) currentPhaseMod -= SINESIZE;

			//L Channel
			phaseIncLeft = SINESIZE * noteFrequencyLeft * powf(range+1, SineModBuff[(uint16_t)currentPhaseMod])/ AUDIO_FREQUENCY_44K;
			currentPhaseLeft += phaseIncLeft;
			if (currentPhaseLeft > SINESIZE) currentPhaseLeft -= SINESIZE;
			int32_t nextSampleLeft = SineBuff[(uint16_t)(currentPhaseLeft)];

			//R Channel
			phaseIncRight = SINESIZE * noteFrequencyRight * powf(range+1, SineModBuff[(uint16_t)currentPhaseMod])/ AUDIO_FREQUENCY_44K;
			currentPhaseRight += phaseIncRight;
			if (currentPhaseRight > SINESIZE) currentPhaseRight -= SINESIZE;
			int32_t nextSampleRight = SineBuff[(uint16_t)(currentPhaseRight)];

			//Volume determined by ampMod(global volume) and L/R pan
			PlayBuff[i] = (uint16_t)((float)nextSampleLeft * ampMod * leftPan);
			PlayBuff[i + 1] = (uint16_t)((float)nextSampleRight * ampMod * rightPan);
			count++;
		}
	}
}

//Set frequency
void changeFrequency(uint32_t newFreq){
	noteFrequencyLeft = newFreq;
	noteFrequencyRight = newFreq;
}

//Change frequency of the modulating frequency
void changeFrequencyMod(float newFreq){
	noteFrequencyMod = newFreq;
	//phaseIncMod = SINESIZE * noteFrequencyMod / AUDIO_FREQUENCY_44K;
}

//Change Amplitude
void changeAmpMod(float newAmpMod){
	ampMod = newAmpMod;
}

//Update Left/Right Panning
void updatePan(uint32_t joystick){
	rightPan = (float)(joystick)/4096;
	leftPan = 1 - rightPan;
}

