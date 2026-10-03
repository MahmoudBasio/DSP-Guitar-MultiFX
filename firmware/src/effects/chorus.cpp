#include "effects/chorus.h"
#include "audio/audio_graph.h"
#include "dsp/parameters.h"
#include "config/constants.h"
#include <math.h>

// --- DSP Class Implementation ---
AudioEffectCustomChorus::AudioEffectCustomChorus() 
    : AudioStream(2, inputQueueArray), writeIndex(0), lfoPhase(0.0f), enabled(true),
      baseDelay(20.0f), depth(10.0f), rate(0.2f), dryMix(0.6f), wetMix(0.8f) {
    memset(delayBuffer, 0, sizeof(delayBuffer));
}

void AudioEffectCustomChorus::setEnabled(bool en) { enabled = en; }

void AudioEffectCustomChorus::setWetFilterEnabled(bool en) { wetFilterEnabled = en; }

void AudioEffectCustomChorus::setParams(float b, float d, float r, float dg, float wg) {
    // Keep both interpolation taps within the available delay history.
    const float maxDelayMs = (BUFFER_SIZE - 2) * 1000.0f / SAMPLE_RATE;
    baseDelay = constrain(b, 1.0f, maxDelayMs);
    depth = constrain(d, 0.0f, fminf(baseDelay - 1.0f, maxDelayMs - baseDelay));
    rate = constrain(r, 0.1f, 5.0f);
    dryMix = constrain(dg, 0.0f, 1.0f);
    wetMix = constrain(wg, 0.0f, 1.0f);
}

void AudioEffectCustomChorus::update(void) {
    audio_block_t *block = receiveWritable(0);
    audio_block_t *filtered = receiveReadOnly(1);
    if (!block) { if (filtered) release(filtered); return; }
    if (!enabled) {
        if (filtered) release(filtered);
        transmit(block); release(block); return;
    }

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        float input = block->data[i] / 32768.0f;
        // Only the delayed component may be filtered; dry audio stays unfiltered.
        delayBuffer[writeIndex] = wetFilterEnabled
            ? (filtered ? filtered->data[i] / 32768.0f : 0.0f) : input;
        
        float lfo = sinf(lfoPhase);
        lfoPhase += 2.0f * PI * rate / SAMPLE_RATE;
        if (lfoPhase > 2.0f * PI) lfoPhase -= 2.0f * PI;
        
        float delaySamples = (baseDelay + depth * lfo) * SAMPLE_RATE / 1000.0f;
        float readPos = writeIndex - delaySamples;
        while (readPos < 0) readPos += BUFFER_SIZE;
        
        int i1 = (int)readPos;
        int i2 = (i1 + 1) % BUFFER_SIZE;
        float f = readPos - i1;
        float delayed = delayBuffer[i1] * (1.0f - f) + delayBuffer[i2] * f;
        
        float output = constrain(input * dryMix + delayed * wetMix, -1.0f, 1.0f);
        block->data[i] = (int16_t)(output * 32767.0f);
        writeIndex = (writeIndex + 1) % BUFFER_SIZE;
    }
    if (filtered) release(filtered);
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
    AudioGraph::chorus.setWetFilterEnabled(currentChorusParams.wet_filter);
    AudioGraph::chorus.setParams(
        currentChorusParams.base_ms, 
        currentChorusParams.depth, 
        currentChorusParams.rate_hz, 
        CHORUS_DRY_GAIN, 
        currentChorusParams.wet
    );
}