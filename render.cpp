#include <Bela.h>
#include <stdio.h>
#include <libraries/Scope/Scope.h>
#include "KarplusStrong.h"

#include <libraries/Trill/Trill.h>

#define NUM_TOUCH 5 // Number of touches on Trill sensor

// Trill object declaration and global variable setup
Trill touchSensor;
int gNumActiveTouches = 0;
float gTouchLocation[NUM_TOUCH] = {0.0, 0.0, 0.0, 0.0, 0.0};
float gTouchSize[NUM_TOUCH] = {0.0, 0.0, 0.0, 0.0, 0.0};
int gCurrTouch = 0;

// Declare remaining objects
Scope gScope;
KarplusStrong ks;

/* Some global flags & variables */
unsigned int gTaskSleepTime = 12000; // microseconds
float gAmplitude = 0.5;              // Amplitude of the playback
int gInputPin = 1;
int gOutputPin = 0;
int lastStatus = LOW;

/*
    The function used as a BELA Aux task to interact with both the TRILL BAR and CURL API
*/
void trill_loop(void *)
{
    while (!Bela_stopRequested())
    {
        // Read locations from Trill sensor
        touchSensor.readI2C();
        gNumActiveTouches = touchSensor.getNumTouches();
        for (unsigned int i = 0; i < gNumActiveTouches; i++)
        {
            // Update the Current Touch Global
            if (gTouchLocation[i] == 0) // If the state is transitioning from LOW -> HIGH
            {
            	ks.setFrequency(gTouchLocation[0] * 440); // This is a very simple scaling of 0..1 values to 0..440hz 
            }
            else
            {
                rt_printf("Still Touching\n");
            }

            gTouchLocation[i] = touchSensor.touchLocation(i);
            gTouchSize[i] = touchSensor.touchSize(i);
        }
        // For all inactive touches, set location and size to 0
        for (unsigned int i = gNumActiveTouches; i < NUM_TOUCH; i++)
        {
            gTouchLocation[i] = 0.0;
            gTouchSize[i] = 0.0;
        }
        usleep(gTaskSleepTime);
    }
}

/*
    The main BELA setup function
*/
bool setup(BelaContext *context, void *userData)
{
    // Setup a Trill Bar sensor on i2c bus 1, using the default mode and address
    if (touchSensor.setup(1, Trill::BAR) != 0)
    {
        fprintf(stderr, "Unable to initialise Trill Bar\n");
        return false;
    }

    touchSensor.printDetails();

    // Set the mode of digital pins
    pinMode(context, 0, gInputPin, INPUT);
    pinMode(context, 0, gOutputPin, OUTPUT);

    // Set up the KSA object
    ks.SetSampleRate(context->audioSampleRate); // Must be called FIRST
    ks.setFrequency(55);                        // Default first tone.
    ks.resetBuffer();

    // Initialise the Bela oscilloscope
    gScope.setup(1, context->audioSampleRate);

    // Set and schedule auxiliary task for reading data from Trill / REST API
    Bela_runAuxiliaryTask(trill_loop);

    // Print the sample rate to the user
    rt_printf("\nSample Rate: %f\n", context->audioSampleRate);

    return true;
}

/*
    The BELA render function, called in a continous loop
*/
void render(BelaContext *context, void *userData)
{
    for (unsigned int n = 0; n < context->digitalFrames; n++)
    {
        float out;
        int status = (gTouchLocation[0] != 0) ? HIGH : LOW;

        digitalWriteOnce(context, n, gOutputPin, status); // write the status to the LED

        if (status == HIGH)
        {
            // Get the next sample from KSA
            out = gAmplitude * ks.process();
        }
        else
        {
            out = 0.0f;
            if (lastStatus == HIGH)
            {
                // Only reset the KSA buffer the very first time the status switches from HIGH -> LOW
                ks.resetBuffer();
            }
        }

        for (unsigned int j = 0; j < context->audioOutChannels; j++)
        {
            audioWrite(context, n, j, out); // write the audio output
        }

        // Update the status of the button
        lastStatus = status;

        // Write the sample to the oscilloscope
        gScope.log(out);
    }
}

/*
    The BELA cleanup function, used to release resources as necessary
*/
void cleanup(BelaContext *context, void *userData)
{
	
}
