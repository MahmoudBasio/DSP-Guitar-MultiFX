#include "audio/mixer.h"
#include "audio/audio_graph.h"
#include "config/constants.h"
#include "dsp/safety.h"

namespace Mixer {
    namespace { bool outputEnabled = false; }

    void init() {
        outputEnabled = false; // Keep both I2S channels muted until setup completes.
        updateFinalOutput();
    }

    void updateDelayInput(bool delayOn, float feedback) {
        feedback = DspSafety::clampFinite(feedback, 0.0f, DELAY_MAX_FEEDBACK);
        // Input + feedback gains sum to one; repeats no longer amplify the sum.
        AudioNoInterrupts();
        AudioGraph::delayInputMixer.gain(0, delayOn ? 1.0f - feedback : 0.0f);
        AudioGraph::delayInputMixer.gain(1, delayOn ? feedback : 0.0f);
        AudioInterrupts();
    }

    void updateReverbInput(bool delayOn, bool reverbOn) {
        const auto gains = DspSafety::normalizedGains(
            (delayOn && reverbOn) ? REVERB_FROM_DELAY_GAIN : 0.0f,
            reverbOn ? REVERB_FROM_CHORUS_GAIN : 0.0f);
        AudioNoInterrupts();
        AudioGraph::reverbInputMixer.gain(0, gains.first);
        AudioGraph::reverbInputMixer.gain(1, gains.second);
        AudioInterrupts();
    }

    void updateWetMix(bool delayOn, float delayWet, bool reverbOn, float reverbWet) {
        const auto gains = DspSafety::normalizedGains(
            delayOn ? delayWet : 0.0f, reverbOn ? reverbWet : 0.0f);
        AudioNoInterrupts();
        AudioGraph::wetMixer.gain(0, gains.first);
        AudioGraph::wetMixer.gain(1, gains.second);
        AudioInterrupts();
    }
    
    void updateFinalOutput() {
        const auto gains = DspSafety::normalizedGains(FINAL_DRY_GAIN, FINAL_WET_GAIN);
        AudioNoInterrupts();
        AudioGraph::finalMixerL.gain(0, outputEnabled ? gains.first : 0.0f);
        AudioGraph::finalMixerR.gain(0, outputEnabled ? gains.first : 0.0f);
        AudioGraph::finalMixerL.gain(1, outputEnabled ? gains.second : 0.0f);
        AudioGraph::finalMixerR.gain(1, outputEnabled ? gains.second : 0.0f);
        AudioInterrupts();
    }

    void setOutputEnabled(bool enabled) {
        outputEnabled = enabled;
        updateFinalOutput();
    }
}
