/***** KarplusStrong.h *****/
#pragma once

#include <stdio.h>
#include <string>
#include <Bela.h>
#include <math.h>

#define MAX_LEN 1000

class KarplusStrong {
public:
	KarplusStrong(void);
	~KarplusStrong(void);
	float process(void);
	float process(int sampleCount);
    void resetBuffer(void);
    void setFrequency(float hz);
    void SetSampleRate(float sampleRate);

protected:
	float gWavetable[MAX_LEN];	// Buffer that holds the wavetable
	int gReadPointer = 0;	// Position of the last frame we played
	int wavetableLen;
	float belaSampleRate;
	float decayConstant = 0.996; // 
	
private:
	// Store references to previous values to compute KarplusStrong
	float FIRST = 0.0;
	float SECOND = 0.0;
};

inline float KarplusStrong::process() {

	float out = gWavetable[gReadPointer];
	
	/* Set initial values for previous entities when the time is right*/
	if(FIRST == 0.0){
		FIRST = gWavetable[gReadPointer];
	}
	else if(SECOND== 0.0){
		SECOND = gWavetable[gReadPointer];
	}
	
	/* Run the KarplusStrong Algorithm here*/
	if(SECOND != 0.0 && FIRST != 0.0){
		SECOND = FIRST;
		FIRST = gWavetable[gReadPointer];
		gWavetable[gReadPointer] = decayConstant * 0.5 * (SECOND + FIRST);
	}
    
    // Increment read pointer and reset to 0 when end of table is reached
    gReadPointer++;
    
    while (gReadPointer >= wavetableLen){
    	gReadPointer -= wavetableLen;
    }
	
	return out;
}

inline float KarplusStrong::process(int sampleCount)
{
	float retVal = 0;

	if(true) // Update as required
	{
		for(int i=0; i<sampleCount; i++)
			retVal = process();
	}

	return retVal;
}

inline void KarplusStrong::resetBuffer() {
    
    /*
    	Fill the buffer with white noise in the range of  -1 and 1.
    */
    for (unsigned int n = 0; n < wavetableLen; n++)
    {
    	float r = (float)rand() / (float)RAND_MAX;
    	gWavetable[n] =  -1 + r * 2;
    }
    
    // Reset the KS values to 0.0
    FIRST = 0.0;
    SECOND = 0.0;
}

inline void KarplusStrong::setFrequency(float hz){
	// use belaSampleRate instead of direct int
	wavetableLen = floor(belaSampleRate / hz);
}
