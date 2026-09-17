#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

namespace Mixer {
    // Initializes the default static gains for the mixers
    void init();

    // Updates the routing and feedback loop into the delay block
    void updateDelayInput(bool delayOn, float feedback);

    // Updates the parallel and serial routing into the reverb block
    void updateReverbInput(bool delayOn, bool reverbOn);

    // Updates the combined wet signals before the final output stage
    void updateWetMix(bool delayOn, float delayWet, bool reverbOn, float reverbWet);

    // Sets the final stereo output dry/wet balances
    void updateFinalOutput();
    void setOutputEnabled(bool enabled);
}

#endif // AUDIO_MIXER_H
