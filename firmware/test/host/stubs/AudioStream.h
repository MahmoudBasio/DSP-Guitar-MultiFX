#pragma once
#include "Arduino.h"
#define AUDIO_BLOCK_SAMPLES 128
#define AUDIO_SAMPLE_RATE_EXACT 44100.0f
struct audio_block_t { int16_t data[AUDIO_BLOCK_SAMPLES]; };

// Only the block transport boundary is mocked, not the custom chorus algorithm.
class AudioStream {
public:
    AudioStream(unsigned char, audio_block_t **) {}
    virtual ~AudioStream() = default;
    virtual void update() = 0;
    void inject(audio_block_t *block) { pending = block; }
    audio_block_t lastOutput = {};
    int transmitted = 0;
protected:
    audio_block_t *receiveWritable(unsigned int) {
        audio_block_t *block = pending;
        pending = nullptr;
        return block;
    }
    void transmit(audio_block_t *block) { lastOutput = *block; ++transmitted; }
    void release(audio_block_t *) {}
private:
    audio_block_t *pending = nullptr;
};
