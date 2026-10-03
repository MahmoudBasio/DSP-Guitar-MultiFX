#include "audio/audio_manager.h"
#include "audio/audio_graph.h"
#include "audio/mixer.h"
#include "config/constants.h"

namespace AudioManager {
    void init() {
        AudioGraph::setup(); // Allocates global memory and I2S
        AudioGraph::audioShield.lineInLevel(LINEIN_LEVEL);
        setupStaticRouting();
        Mixer::init();
    }

    void setSystemVolume(float volume) {
        AudioGraph::audioShield.volume(volume);
    }

    void setupStaticRouting() {
        AudioGraph::chorusWetFilter.setLowpass(0, CHORUS_WET_FILTER_HZ, 0.707f);
        AudioGraph::repeatFilter.setLowpass(0, REPEAT_FILTER_HZ, 0.707f);
    }
}