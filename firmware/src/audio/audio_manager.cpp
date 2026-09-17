#include "audio/audio_manager.h"
#include "audio/audio_graph.h"
#include "audio/mixer.h"
#include "config/constants.h"
#include "dsp/safety.h"

namespace AudioManager {
    void init() {
        // Mute before enabling the codec, including when booting into standby.
        Mixer::init();
        setupStaticRouting();
        AudioGraph::setup();
    }

    void setSystemVolume(float volume) {
        volume = DspSafety::clampFinite(volume, 0.0f, 1.0f);
        // SGTL5000 volume() affects headphones, not line-out. Mute in the graph too.
        Mixer::setOutputEnabled(volume > 0.0f);
        AudioGraph::audioShield.volume(volume);
    }

    void setupStaticRouting() {
        AudioGraph::preFilter.setLowpass(0, PRE_FILTER_HZ, 0.707f);
        AudioGraph::repeatFilter.setLowpass(0, REPEAT_FILTER_HZ, 0.707f);
    }
}
