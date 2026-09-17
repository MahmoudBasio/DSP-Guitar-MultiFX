#include <cassert>
#include <cfloat>
#include <iostream>
#include <limits>
#include "audio/audio_graph.h"
#include "audio/audio_manager.h"
#include "audio/mixer.h"
#include "config/constants.h"
#include "config/pins.h"
#include "core/debounce.h"
#include "core/diagnostics.h"
#include "core/system.h"
#include "dsp/safety.h"
#include "dsp/parameters.h"
#include "effects/effect_manager.h"
#include "ui/encoder.h"
#include "ui/ui.h"
#include "ui/menu.h"

namespace TestHardware {
uint32_t now = 0;
int pins[64] = {};
long encoder = 0;
std::string serialOutput, serialInput;
}
namespace TestAudio {
bool locked = false;
int poolBlocks = 0;
float cpu = 3.0f, cpuMax = 5.0f;
unsigned int memory = 200, memoryMax = 300;
}
MockSerial Serial;
void setup();
void loop();

bool near(float a, float b) { return std::fabs(a - b) < 0.001f; }
void resetInputs() {
    for (int &pin : TestHardware::pins) pin = HIGH;
}

void testDebounce() {
    DebouncedInput a, b;
    a.begin(false, 0); b.begin(false, 0);
    assert(!a.update(true, 10, 30));
    assert(!a.update(false, 12, 30));
    assert(!a.update(true, 15, 30));
    assert(!a.update(true, 44, 30));
    assert(a.update(true, 45, 30) && a.isPressed());
    assert(!a.update(true, 100, 30)); // held button: no retrigger
    assert(!b.update(true, 20, 30));
    assert(b.update(true, 50, 30)); // other switch has an independent timer
    assert(!a.update(false, 101, 30));
    assert(a.update(false, 131, 30) && !a.isPressed());
    a.begin(false, UINT32_MAX - 20);
    assert(!a.update(true, UINT32_MAX - 10, 30));
    assert(a.update(true, 20, 30)); // 31 ms across wrap
}

void testParameterSweep() {
    for (int base = 0; base <= 100; ++base) {
        for (int depth = 0; depth <= 100; ++depth) {
            values[2][2] = base; values[2][1] = depth;
            EffectManager::applySettings();
            const float b = currentChorusParams.base_ms;
            const float d = currentChorusParams.depth;
            assert(b >= 10.0f && b <= 30.0f);
            assert((b - d) * 44.1f >= 1.0f - 0.001f);
            assert((b + d) * 44.1f <= CHORUS_BUFFER_SAMPLES - 2 + 0.001f);
            for (float lfo : {-1.0f, 0.0f, 1.0f}) {
                const float delay = DspSafety::chorusDelaySamples(b, d, lfo);
                assert(delay >= 1.0f && delay <= CHORUS_BUFFER_SAMPLES - 2);
            }
        }
    }
    values[0][0] = -20; values[0][1] = 120; values[2][0] = 999;
    EffectManager::applySettings();
    assert(currentDelayParams.time_ms == 10);
    assert(near(currentDelayParams.feedback, 0.95f));
    assert(near(currentChorusParams.rate_hz, 5.0f));
    values[0][0] = 100;
    EffectManager::toggleDelay();
    EffectManager::delay.updateModulation(FLT_MAX);
    assert(near(AudioGraph::delay1.lastDelay, 1003.0f));
    assert(DspSafety::delayBlocksForMs(1003.0f) + AUDIO_GRAPH_RESERVE_BLOCKS <= AUDIO_MEMORY_BLOCKS);
    assert(TestAudio::poolBlocks == AUDIO_MEMORY_BLOCKS);
    EffectManager::resetToDefaults();
    assert(!EffectManager::delay.isEnabled() && !EffectManager::reverb.isEnabled()
           && !EffectManager::chorus.isEnabled());
    assert(near(currentChorusParams.base_ms, 20.0f));
    assert(near(currentChorusParams.depth, 19.8f));
    assert(currentDelayParams.time_ms == 425);
}

void testChorusProcessing() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    const float settings[][5] = {
        {10, 30, 5, 0.6f, 0.8f}, {30, 30, 5, 0.6f, 1},
        {20, 19.8f, 0.3f, 0.6f, 0.8f}, {10, 0, 5, 1, 0},
        {-FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
        {nan, inf, nan, inf, nan}
    };
    for (const auto &p : settings) {
        AudioEffectCustomChorus chorus;
        chorus.setParams(p[0], p[1], p[2], p[3], p[4]);
        chorus.setEnabled(true);
        audio_block_t block = {};
        // More than a full LFO cycle even for the 0.1 Hz fallback.
        for (int n = 0; n < 4000; ++n) {
            for (int16_t &sample : block.data) sample = n % 2 ? -32768 : 32767;
            chorus.inject(&block); chorus.update();
        }
        assert(chorus.transmitted == 4000);
        chorus.setEnabled(false);
        for (int16_t &sample : block.data) sample = 1234;
        chorus.inject(&block); chorus.update();
        for (int16_t sample : chorus.lastOutput.data) assert(sample == 1234);
        chorus.update(); // No input: no output.
        assert(chorus.transmitted == 4001);
    }

    // With normalized gains, a coherent full-scale sum must not hard-clip.
    AudioEffectCustomChorus chorus;
    chorus.setParams(20, 0, 1, 0.6f, 0.8f);
    chorus.setEnabled(true);
    audio_block_t block = {};
    for (int n = 0; n < 30; ++n) {
        for (int16_t &sample : block.data) sample = 30000;
        chorus.inject(&block); chorus.update();
    }
    for (int16_t sample : chorus.lastOutput.data) assert(sample >= 29998 && sample <= 30000);

    // A bypassed chorus must not keep stale positive samples to replay later.
    chorus.setEnabled(false);
    for (int n = 0; n < 30; ++n) {
        for (int16_t &sample : block.data) sample = -12000;
        chorus.inject(&block); chorus.update();
    }
    chorus.setParams(20, 0, 1, 0, 1);
    chorus.setEnabled(true);
    for (int16_t &sample : block.data) sample = -12000;
    chorus.inject(&block); chorus.update();
    for (int16_t sample : chorus.lastOutput.data) assert(sample >= -12000 && sample <= -11998);
}

void testMixers() {
    for (float gain : {0.0f, 0.2f, 0.95f, 1.0f}) {
        Mixer::updateDelayInput(true, gain);
        assert(near(AudioGraph::delayInputMixer.gains[0] + AudioGraph::delayInputMixer.gains[1], 1));
        Mixer::updateWetMix(true, gain, true, 1);
        assert(AudioGraph::wetMixer.gains[0] + AudioGraph::wetMixer.gains[1] <= 1.00001f);
    }
    Mixer::updateReverbInput(true, true);
    assert(near(AudioGraph::reverbInputMixer.gains[0] + AudioGraph::reverbInputMixer.gains[1], 1));
    Mixer::updateWetMix(false, 1, false, 1);
    assert(AudioGraph::wetMixer.gains[0] == 0 && AudioGraph::wetMixer.gains[1] == 0);
    AudioManager::setSystemVolume(0);
    assert(AudioGraph::finalMixerL.gains[0] == 0 && AudioGraph::finalMixerL.gains[1] == 0);
    assert(AudioGraph::finalMixerR.gains[0] == 0 && AudioGraph::finalMixerR.gains[1] == 0);
    AudioManager::setSystemVolume(0.7f);
    assert(near(AudioGraph::finalMixerL.gains[0] + AudioGraph::finalMixerL.gains[1], 1));
}

void testControls() {
    resetInputs(); TestHardware::now = 100;
    EffectManager::resetToDefaults();
    TestHardware::pins[PIN_SYS_BTN] = LOW;
    initSystem();
    assert(system_is_on && TestHardware::pins[PIN_SYS_LED] == HIGH);
    TestHardware::pins[PIN_FS_DELAY] = LOW;
    pollFootswitches();
    TestHardware::now = 110; TestHardware::pins[PIN_FS_REVERB] = LOW;
    pollFootswitches();
    TestHardware::now = 140; pollFootswitches();
    assert(EffectManager::delay.isEnabled() && EffectManager::reverb.isEnabled());
    TestHardware::now = 200; pollFootswitches();
    assert(EffectManager::delay.isEnabled()); // still held
    TestHardware::pins[PIN_SYS_BTN] = HIGH;
    cb_SystemCheck();
    TestHardware::now = 240; cb_SystemCheck();
    assert(!system_is_on && AudioGraph::finalMixerL.gains[0] == 0);
    TestHardware::pins[PIN_FS_CHORUS] = LOW; pollFootswitches();
    TestHardware::now = 280; pollFootswitches();
    assert(!EffectManager::chorus.isEnabled());
    TestHardware::pins[PIN_SYS_BTN] = LOW; cb_SystemCheck();
    TestHardware::now = 320; cb_SystemCheck(); pollFootswitches();
    assert(!EffectManager::chorus.isEnabled()); // no queued standby press
    main_index = 0; current_screen = MAIN_MENU;
    pollEncoder();
    assert(main_index == 0); // no phantom startup rotation
    TestHardware::encoder = 4; pollEncoder();
    assert(main_index == 1);
    main_index = 3; handleButtonLogic();
    assert(current_screen == MAIN_MENU);
    assert(!EffectManager::delay.isEnabled());
}

void testDiagnostics() {
#if MULTIFX_DIAGNOSTICS
    Diagnostics::init();
    TestHardware::serialInput = "r";
    Diagnostics::poll();
    assert(TestHardware::serialOutput.find("estimated_max_block_us") != std::string::npos);
    TestHardware::now += 1001;
    Diagnostics::poll();
    assert(TestHardware::serialOutput.find(",400,") != std::string::npos);
#endif
}

int main() {
    resetInputs(); setup();
    assert(!system_is_on); // pulled-up master input means standby
    assert(AudioGraph::finalMixerR.gains[1] == 0);
    testDebounce();
    testParameterSweep();
    testChorusProcessing();
    testMixers();
    testControls();
    testDiagnostics();
    loop();
    assert(!TestAudio::locked);
    std::cout << "PASS: debounce, 10,201 chorus settings, DSP boundary/nonfinite cases, "
                 "delay budget, routing gains, controls, and diagnostics\n";
}
