#include "core/diagnostics.h"

#if MULTIFX_DIAGNOSTICS
#include <Arduino.h>
#include <Audio.h>
#include <stdio.h>
#include "config/constants.h"
#include "core/system.h"
#include "dsp/parameters.h"
#include "effects/effect_manager.h"

namespace Diagnostics {
namespace {
uint32_t lastReport = 0;

void resetWindow() {
    AudioNoInterrupts();
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
    AudioInterrupts();
    lastReport = millis();
}

void printHeader() {
    Serial.printf("# build=%s %s, sample_hz=%.1f, block_samples=%d, cpu_hz=%lu\n",
                  __DATE__, __TIME__, AUDIO_SAMPLE_RATE_EXACT, AUDIO_BLOCK_SAMPLES,
                  static_cast<unsigned long>(F_CPU_ACTUAL));
    Serial.println("# Peaks are since reset; block time is derived from audio CPU, not measured latency.");
    Serial.println("time_ms,system_on,delay_on,reverb_on,chorus_on,delay_ms,feedback,delay_wet,room,damping,reverb_wet,chorus_base_ms,chorus_depth_ms,chorus_rate_hz,cpu_percent,cpu_max_percent,audio_blocks,audio_blocks_max,pool_blocks,estimated_max_block_us");
}
}

void init() { resetWindow(); }

void poll() {
    if (!Serial) return;
    // Limit work per foreground loop even if a host sends a long serial stream.
    if (Serial.available() && Serial.read() == 'r') {
        printHeader();
        resetWindow();
    }
    const uint32_t now = millis();
    if (static_cast<uint32_t>(now - lastReport) < 1000) return;
    lastReport = now;

    AudioNoInterrupts();
    const float cpu = AudioProcessorUsage();
    const float cpuMax = AudioProcessorUsageMax();
    const unsigned int memory = AudioMemoryUsage();
    const unsigned int memoryMax = AudioMemoryUsageMax();
    AudioInterrupts();

    const float blockUs = 1000000.0f * AUDIO_BLOCK_SAMPLES / AUDIO_SAMPLE_RATE_EXACT;
    char row[320];
    const int length = snprintf(row, sizeof(row),
        "%lu,%d,%d,%d,%d,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%u,%u,%d,%.4f\n",
        static_cast<unsigned long>(now), system_is_on,
        EffectManager::delay.isEnabled(), EffectManager::reverb.isEnabled(),
        EffectManager::chorus.isEnabled(), currentDelayParams.time_ms,
        currentDelayParams.feedback, currentDelayParams.wet,
        currentReverbParams.room_size, currentReverbParams.damping, currentReverbParams.wet,
        currentChorusParams.base_ms, currentChorusParams.depth, currentChorusParams.rate_hz,
        cpu, cpuMax, memory, memoryMax, AUDIO_MEMORY_BLOCKS, cpuMax * blockUs / 100.0f);
    // Skip a row rather than filling a disconnected/slow USB host's TX buffer.
    if (length > 0 && length < static_cast<int>(sizeof(row)) && Serial.availableForWrite() >= length) {
        Serial.write(reinterpret_cast<const uint8_t *>(row), length);
    }
}
}
#endif
