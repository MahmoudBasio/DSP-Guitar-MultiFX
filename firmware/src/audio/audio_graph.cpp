#include "audio/audio_graph.h"

namespace AudioGraph {
    // Instantiate raw objects
    AudioInputI2S            in;
    AudioFilterBiquad        chorusWetFilter;
    AudioEffectCustomChorus  chorus;
    AudioMixer4              delayInputMixer;
    AudioEffectDelay         delay1;
    AudioFilterBiquad        repeatFilter;
    AudioMixer4              reverbInputMixer;
    AudioEffectFreeverb      reverb1;
    AudioMixer4              wetMixer;
    AudioMixer4              finalMixerL;
    AudioMixer4              finalMixerR;
    AudioOutputI2S           out;
    AudioControlSGTL5000     audioShield;

    // Static Patch Cords
    AudioConnection patchCord1(in, 0, chorus, 0);
    AudioConnection chorusFilterInput(in, 0, chorusWetFilter, 0);
    AudioConnection patchCord2(chorusWetFilter, 0, chorus, 1);
    AudioConnection patchCord3(chorus, 0, finalMixerL, 0);
    AudioConnection patchCord4(chorus, 0, finalMixerR, 0);
    AudioConnection patchCord5(chorus, 0, delayInputMixer, 0);
    AudioConnection patchCord6(delayInputMixer, 0, delay1, 0);
    AudioConnection patchCord7(delay1, 0, repeatFilter, 0);
    AudioConnection patchCord8(repeatFilter, 0, delayInputMixer, 1);
    AudioConnection patchCord9(repeatFilter, 0, wetMixer, 0);
    AudioConnection patchCord10(repeatFilter, 0, reverbInputMixer, 0); 
    AudioConnection patchCord11(chorus, 0, reverbInputMixer, 1); 
    AudioConnection patchCord12(reverbInputMixer, 0, reverb1, 0);
    AudioConnection patchCord13(reverb1, 0, wetMixer, 1);
    AudioConnection patchCord14(wetMixer, 0, finalMixerL, 1);
    AudioConnection patchCord15(wetMixer, 0, finalMixerR, 1);
    AudioConnection patchCord16(finalMixerL, 0, out, 0);
    AudioConnection patchCord17(finalMixerR, 0, out, 1);

    void setup() {
        AudioMemory(300);
        audioShield.enable();
        audioShield.inputSelect(AUDIO_INPUT_LINEIN);
    }
}