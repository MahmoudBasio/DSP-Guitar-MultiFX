#include "effects/reverb.h"
#include "audio/audio_graph.h"
#include "dsp/parameters.h"
#include "dsp/safety.h"

ReverbEffect::ReverbEffect() : enabled(false) {}

void ReverbEffect::setEnabled(bool state) {
    enabled = state;
    updateParameters();
}

bool ReverbEffect::isEnabled() const {
    return enabled;
}

void ReverbEffect::updateParameters() {
    const float room = DspSafety::clampFinite(currentReverbParams.room_size, 0.0f, 1.0f);
    const float damping = DspSafety::clampFinite(currentReverbParams.damping, 0.0f, 1.0f);
    AudioNoInterrupts();
    AudioGraph::reverb1.roomsize(room);
    AudioGraph::reverb1.damping(damping);
    AudioInterrupts();
}
