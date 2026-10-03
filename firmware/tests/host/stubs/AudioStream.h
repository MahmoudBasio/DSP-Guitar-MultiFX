#pragma once
#include <cstdint>
#include <cassert>
#define AUDIO_BLOCK_SAMPLES 128
struct audio_block_t { int16_t data[AUDIO_BLOCK_SAMPLES] = {}; };
class AudioStream {
public:
    audio_block_t* incoming[2] = {};
    audio_block_t output;
    int released = 0, transmitted = 0;
    AudioStream(int, audio_block_t**) {}
    virtual void update() = 0;
    audio_block_t* receiveReadOnly(int i) { auto p = incoming[i]; incoming[i] = nullptr; return p; }
    audio_block_t* receiveWritable(int i) { return receiveReadOnly(i); }
    void release(audio_block_t* p) { assert(p); ++released; }
    void transmit(audio_block_t* p) { output = *p; ++transmitted; }
};
