#ifndef AUDIO_GRAPH_H
#define AUDIO_GRAPH_H

#include <Audio.h>
#include "effects/chorus.h" // Required for the raw AudioEffectCustomChorus definition

namespace AudioGraph {
    // Expose raw audio objects 
    extern AudioInputI2S            in;
    extern AudioEffectCustomChorus  chorus;
    extern AudioFilterBiquad        chorusWetFilter;
    extern AudioMixer4              delayInputMixer;
    extern AudioEffectDelay         delay1;
    extern AudioFilterBiquad        repeatFilter;
    extern AudioMixer4              reverbInputMixer;
    extern AudioEffectFreeverb      reverb1;
    extern AudioMixer4              wetMixer;
    extern AudioMixer4              finalMixerL;
    extern AudioMixer4              finalMixerR;
    extern AudioOutputI2S           out;
    extern AudioControlSGTL5000     audioShield;

    // Allocate audio memory and initialize raw connections
    void setup();
}

#endif // AUDIO_GRAPH_H