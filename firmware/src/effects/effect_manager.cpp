#include "effects/effect_manager.h"
#include "audio/mixer.h"
#include "dsp/parameters.h"
#include "config/presets.h"
#include "dsp/dsp.h"
#include "ui/ui.h"
#include <Audio.h>
#include <math.h>

DelayEffect EffectManager::delay;
ReverbEffect EffectManager::reverb;
ChorusEffect EffectManager::chorus;

void EffectManager::init() {
    resetToDefaults();
}

void EffectManager::applySettings() {
    currentDelayParams.time_ms = (int)mapFloat(values[0][0], 0, 100, 10.0, 1000.0);
    currentDelayParams.feedback = mapFloat(values[0][1], 0, 100, 0.0, 0.95);
    currentDelayParams.wet = mapFloat(values[0][2], 0, 100, 0.0, 1.0);

    currentReverbParams.room_size = mapFloat(values[1][0], 0, 100, 0.0, 1.0);
    currentReverbParams.damping = mapFloat(values[1][1], 0, 100, 0.0, 1.0);
    currentReverbParams.wet = mapFloat(values[1][2], 0, 100, 0.0, 1.0);

    currentChorusParams.rate_hz = mapFloat(values[2][0], 0, 100, 0.1, 5.0);

    currentChorusParams.base_ms = mapFloat(values[2][2], 0, 100, 10.0, 30.0);

    // Keep UI values and DSP delay bounds consistent (minimum delay: 1 ms).
    int maxDepthValue = (int)floorf((currentChorusParams.base_ms - 1.0f) * 100.0f / 30.0f);
    values[2][1] = constrain(values[2][1], 0, min(100, maxDepthValue));
    currentChorusParams.depth = mapFloat(values[2][1], 0, 100, 0.0, 30.0);
    currentChorusParams.wet = values[2][3] / 100.0f;
    currentChorusParams.wet_filter = values[2][4] != 0;

    // Publish a coherent set of parameters between audio updates.
    AudioNoInterrupts();
    delay.updateParameters();
    reverb.updateParameters();
    chorus.updateParameters();

    Mixer::updateReverbInput(delay.isEnabled(), reverb.isEnabled());
    Mixer::updateWetMix(delay.isEnabled(), currentDelayParams.wet, reverb.isEnabled(), currentReverbParams.wet);
    AudioInterrupts();
}

void EffectManager::resetToDefaults() {
    for(int i=0; i<3; i++) {
        for(int j=0; j<parameterCount(i); j++) {
            values[i][j] = preset_values[i][j];
        }
    }
    delay.setEnabled(false);
    reverb.setEnabled(false);
    chorus.setEnabled(false);
    applySettings();
    ui_needs_update = true;
}

void EffectManager::toggleDelay() {
    delay.setEnabled(!delay.isEnabled());
    applySettings();
    ui_needs_update = true;
}

void EffectManager::toggleReverb() {
    reverb.setEnabled(!reverb.isEnabled());
    applySettings();
    ui_needs_update = true;
}

void EffectManager::toggleChorus() {
    chorus.setEnabled(!chorus.isEnabled());
    applySettings();
    ui_needs_update = true;
}