#include "effects/chorus.h"
#include "audio/audio_graph.h"
#include "dsp/parameters.h"
#include "config/constants.h"
#include "dsp/safety.h"
#include <math.h>

// --- DSP Class Implementation ---
AudioEffectCustomChorus::AudioEffectCustomChorus() 
    : AudioStream(1, inputQueueArray), writeIndex(0), lfoPhase(0.0f), enabled(false),
      baseDelay(20.0f), depth(10.0f), rate(0.2f), dryMix(0.6f / 1.4f), wetMix(0.8f / 1.4f) {
    memset(delayBuffer, 0, sizeof(delayBuffer));
}

void AudioEffectCustomChorus::setEnabled(bool en) {
    AudioNoInterrupts();
    enabled = en;
    AudioInterrupts();
}

void AudioEffectCustomChorus::setParams(float b, float d, float r, float dg, float wg) {
    const float safeBase = DspSafety::clampFinite(b, CHORUS_MIN_BASE_MS, CHORUS_MAX_BASE_MS);
    const float safeDepth = DspSafety::clampFinite(d, 0.0f, DspSafety::chorusDepthLimit(safeBase));
    const float safeRate = DspSafety::clampFinite(r, 0.1f, 5.0f);
    const auto gains = DspSafety::normalizedGains(dg, wg);
    // Publish the complete parameter set between audio updates, never mid-block.
    AudioNoInterrupts();
    baseDelay = safeBase; depth = safeDepth; rate = safeRate;
    dryMix = gains.first; wetMix = gains.second;
    AudioInterrupts();
}

void AudioEffectCustomChorus::update(void) {
    audio_block_t *block = receiveWritable(0);
    if (!block) return;
    if (!enabled) {
        // Keep history current so re-enabling cannot replay an old frozen buffer.
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i) {
            delayBuffer[writeIndex] = block->data[i] / 32768.0f;
            writeIndex = (writeIndex + 1) % BUFFER_SIZE;
        }
        transmit(block); release(block); return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float input = block->data[i] / 32768.0f;
        delayBuffer[writeIndex] = input;
        
        float lfo = sinf(lfoPhase);
        lfoPhase += 2.0f * PI * rate / SAMPLE_RATE;
        if (lfoPhase > 2.0f * PI) lfoPhase -= 2.0f * PI;
        
        float delaySamples = DspSafety::chorusDelaySamples(baseDelay, depth, lfo);
        float readPos = writeIndex - delaySamples;
        if (readPos < 0) readPos += BUFFER_SIZE;
        
        int i1 = (int)readPos;
        int i2 = (i1 + 1) % BUFFER_SIZE;
        float f = readPos - i1;
        float delayed = delayBuffer[i1] * (1.0f - f) + delayBuffer[i2] * f;
        
        float output = constrain(input * dryMix + delayed * wetMix, -1.0f, 1.0f);
        block->data[i] = (int16_t)(output * 32767.0f);
        writeIndex = (writeIndex + 1) % BUFFER_SIZE;
    }
    transmit(block);
    release(block);
}

// --- Wrapper Class Implementation ---
ChorusEffect::ChorusEffect() : enabled(false) {}

void ChorusEffect::setEnabled(bool state) {
    enabled = state;
    AudioGraph::chorus.setEnabled(state);
}

bool ChorusEffect::isEnabled() const { return enabled; }

void ChorusEffect::updateParameters() {
    AudioGraph::chorus.setParams(
        currentChorusParams.base_ms, 
        currentChorusParams.depth, 
        currentChorusParams.rate_hz, 
        CHORUS_DRY_GAIN, 
        currentChorusParams.wet
    );
}
