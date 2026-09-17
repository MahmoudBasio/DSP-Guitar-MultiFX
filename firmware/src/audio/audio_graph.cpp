#include "audio/audio_graph.h"
#include "config/constants.h"
#include "dsp/safety.h"

static_assert(AUDIO_BLOCK_SAMPLES == DSP_BLOCK_SAMPLES, "Re-budget memory for a new block size");
static_assert(AUDIO_SAMPLE_RATE_EXACT == DSP_SAMPLE_RATE_HZ, "Re-budget delay buffers for a new sample rate");

namespace AudioGraph {
    // Instantiate raw objects
    AudioInputI2S            in;
    AudioFilterBiquad        preFilter;
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
    AudioConnection patchCord1(in, 0, preFilter, 0);
    AudioConnection patchCord2(preFilter, 0, chorus, 0);
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
        AudioMemory(AUDIO_MEMORY_BLOCKS);
        audioShield.enable();
        audioShield.inputSelect(AUDIO_INPUT_LINEIN);
        audioShield.lineInLevel(LINEIN_LEVEL);
    }
}
