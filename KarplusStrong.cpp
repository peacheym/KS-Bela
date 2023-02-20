/***** KarplusStrong.cpp *****/
#include "KarplusStrong.h"
#include <Bela.h>
#include <math.h>


KarplusStrong::KarplusStrong(void) {
    resetBuffer();
}

KarplusStrong::~KarplusStrong(void) {
}


void KarplusStrong::SetSampleRate(float sampleRate){
	belaSampleRate = sampleRate;
}
