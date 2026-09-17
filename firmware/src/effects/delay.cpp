#include "effects/delay.h"
#include "audio/audio_graph.h"
#include "audio/mixer.h"
#include "dsp/parameters.h"
#include "dsp/safety.h"

DelayEffect::DelayEffect() : enabled(false) {}

void DelayEffect::setEnabled(bool state) {
    enabled = state;
    updateParameters();
}

bool DelayEffect::isEnabled() const {
    return enabled;
}

void DelayEffect::updateParameters() {
    const float delayMs = DspSafety::clampFinite(currentDelayParams.time_ms, DELAY_MIN_MS, DELAY_MAX_MS);
    AudioNoInterrupts();
    AudioGraph::delay1.delay(0, delayMs);
    AudioInterrupts();
    Mixer::updateDelayInput(enabled, currentDelayParams.feedback);
}

void DelayEffect::updateModulation(float mod_ms) {
    if (enabled) {
        const float modulation = DspSafety::clampFinite(mod_ms, -DELAY_MOD_DEPTH_MS, DELAY_MOD_DEPTH_MS);
        const float base = DspSafety::clampFinite(currentDelayParams.time_ms, DELAY_MIN_MS, DELAY_MAX_MS);
        const float dynamicDelay = base + modulation;
        AudioNoInterrupts();
        AudioGraph::delay1.delay(0, dynamicDelay);
        AudioInterrupts();
    }
}
