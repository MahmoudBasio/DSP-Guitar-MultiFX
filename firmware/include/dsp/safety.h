#ifndef DSP_SAFETY_H
#define DSP_SAFETY_H

#include <cmath>
#include "config/constants.h"

// Pure helpers shared by firmware and host regression tests.
namespace DspSafety {
inline float clampFinite(float value, float low, float high) {
    if (!std::isfinite(value)) return low;
    return value < low ? low : (value > high ? high : value);
}

inline float chorusDepthLimit(float baseMs) {
    const float minMs = 1000.0f / DSP_SAMPLE_RATE_HZ;
    const float maxMs = (CHORUS_BUFFER_SAMPLES - 2) * minMs;
    const float lowerRoom = baseMs - minMs;
    const float upperRoom = maxMs - baseMs;
    return clampFinite(lowerRoom < upperRoom ? lowerRoom : upperRoom,
                       0.0f, CHORUS_MAX_DEPTH_MS);
}

inline float chorusDelaySamples(float baseMs, float depthMs, float lfo) {
    return clampFinite((baseMs + depthMs * lfo) * DSP_SAMPLE_RATE_HZ / 1000.0f,
                       1.0f, static_cast<float>(CHORUS_BUFFER_SAMPLES - 2));
}

struct GainPair { float first; float second; };

// Preserve the requested ratio, but never amplify a coherent full-scale sum.
inline GainPair normalizedGains(float first, float second) {
    first = clampFinite(first, 0.0f, 1.0f);
    second = clampFinite(second, 0.0f, 1.0f);
    const float total = first + second;
    if (total > 1.0f) { first /= total; second /= total; }
    return {first, second};
}

constexpr int delayBlocksForMs(float milliseconds) {
    return (static_cast<int>(milliseconds * DSP_SAMPLE_RATE_HZ / 1000.0f + 0.5f)
            + DSP_BLOCK_SAMPLES - 1) / DSP_BLOCK_SAMPLES + 1;
}

static_assert(delayBlocksForMs(DELAY_MAX_MS + DELAY_MOD_DEPTH_MS)
              + AUDIO_GRAPH_RESERVE_BLOCKS <= AUDIO_MEMORY_BLOCKS,
              "Audio pool cannot hold maximum modulated delay plus graph reserve");
}

#endif
