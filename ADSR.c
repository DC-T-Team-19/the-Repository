/*
 * ADSR.c
 *
 *  Created on: 10 Mar 2021
 *      Author: JoshM, DavidM, MaxB
 */

//need to include Sam's code for button control, hopefully there is a boolean value for the button status?

#include "ADSR.h"
#include <math.h>
#include <stdio.h>

#define ASIZE 1024
#define DSIZE 1024
#define RSIZE 1024

float aBuff[ASIZE];
float dBuff[DSIZE];
float rBuff[RSIZE];

enum eADSRstatus {attack, decay, sustain, release, off }  ADSRstatus = off;
enum eButtonStatus {buttonDown, buttonRelease} ButtonStatus = buttonRelease;

//Creating a y = x ramp for the Attack, goes to max 1
float* createATable() {
	float q;
	for (int i = 0; i <= ASIZE; i++) {
		if (q != 1) {
			q = i / ASIZE;
			aBuff[i] = (float)q;
		}

	}
	return aBuff;
}

//Creating a y = -x ramp for the Decay, goes from max 1 to the value of the sustain
void createDTable(float constantSustain) {
	float q;
	for (int i = 0; i <= DSIZE; i++) {
		while (q != constantSustain) {
			q = 1 - i / DSIZE;
			dBuff[i] = (float)q;
		}
		ADSRstatus = sustain;
	}
}

//Creating a y = -x ramp for the Release, goes from max sustain value to 0
void createRTable(float constantSustain) {
	float q;
	for (int i = 0; i <= RSIZE; i++) {
		while (q != 0) {
			q = constantSustain - i/RSIZE;
			rBuff[i] = (float)q;
		}
		ADSRstatus = off;
	}
}

//Status changes for the ADSR
void changeADSR() {
	//button pressed
	if (ButtonStatus == buttonDown) {
		ADSRstatus = attack;
	}
	//button released
	if (ButtonStatus == buttonRelease) {
		ADSRstatus = release;
	}
}

