#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <Arduino.h>
#include <AudioStream.h>
// Inspect delay bounds as well as public audio behavior.
#define private public
#include "effects/chorus.h"
#undef private
#include "dsp/parameters.h"
namespace AudioGraph { AudioEffectCustomChorus chorus; }
ChorusParameters currentChorusParams = {};

static void feed(AudioEffectCustomChorus& effect, int16_t raw, int16_t filtered) {
    audio_block_t a, b;
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i) { a.data[i] = raw; b.data[i] = filtered; }
    effect.incoming[0] = &a; effect.incoming[1] = &b;
    effect.update();
}

int main() {
    AudioEffectCustomChorus effect;
    effect.setEnabled(false);
    feed(effect, -32768, 12000);
    assert(effect.output.data[0] == -32768); // Disabled means exact pass-through.
    assert(effect.released == 2 && effect.transmitted == 1);
    audio_block_t orphan;
    effect.incoming[1] = &orphan;
    effect.update();
    assert(effect.released == 3 && effect.transmitted == 1);

    effect.setEnabled(true);
    effect.setParams(10, 0, 1, 1, 0);
    effect.setWetFilterEnabled(true);
    feed(effect, 12000, -20000);
    assert(std::abs(effect.output.data[0] - 12000) <= 1); // Wet filter cannot filter dry.

    for (bool filter : {false, true}) {
        effect.setWetFilterEnabled(filter);
        effect.setParams(10, 0, 1, 0, 1);
        for (int i = 0; i < 40; ++i) feed(effect, 12000, -6000);
        assert(std::abs(effect.output.data[127] - (filter ? -6000 : 12000)) <= 1);
    }
    // Exercise all UI base/depth settings, plus out-of-range direct callers.
    for (int b = -10; b <= 110; ++b) {
        for (int d = -10; d <= 110; ++d) {
            effect.setParams((float)b, (float)d, 1, .6f, .8f);
            assert(effect.baseDelay - effect.depth >= .9999f);
            assert((effect.baseDelay + effect.depth) * 44.1f <= effect.BUFFER_SIZE - 1);
            for (float phase : {0.0f, PI / 2, PI, 3 * PI / 2}) {
                effect.lfoPhase = phase;
                feed(effect, 1000, 500);
            }
        }
    }
    std::cout << "PASS: bypass, block release, dry/wet isolation, filter selection, delay bounds\n";
}
