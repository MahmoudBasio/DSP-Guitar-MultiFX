#ifndef EFFECTS_CHORUS_H
#define EFFECTS_CHORUS_H

#include <Arduino.h>
#include <AudioStream.h>
#include "effects/ieffect.h"

// 1. Raw DSP Object
class AudioEffectCustomChorus : public AudioStream {
public:
    AudioEffectCustomChorus();
    void setEnabled(bool en);
    void setWetFilterEnabled(bool en);
    void setParams(float baseDelayMs, float depthMs, float rateHz, float dryGain, float wetGain);
    virtual void update(void) override;
private:
    audio_block_t *inputQueueArray[2];
    static const int BUFFER_SIZE = 4096;
    float delayBuffer[BUFFER_SIZE];
    int writeIndex;
    float lfoPhase;
    bool enabled;
    bool wetFilterEnabled = false;
    float baseDelay;
    float depth;
    float rate;
    float dryMix;
    float wetMix;
    const float SAMPLE_RATE = 44100.0f;
};

// 2. High-Level Plugin Wrapper
class ChorusEffect : public IEffect {
public:
    ChorusEffect();
    void setEnabled(bool state) override;
    bool isEnabled() const override;
    void updateParameters() override;
private:
    bool enabled;
};

#endif // EFFECTS_CHORUS_H
