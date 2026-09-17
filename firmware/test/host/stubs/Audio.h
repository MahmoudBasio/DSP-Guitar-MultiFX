#pragma once
#include "AudioStream.h"
#include <cassert>
namespace TestAudio {
extern bool locked;
extern int poolBlocks;
extern float cpu, cpuMax;
extern unsigned int memory, memoryMax;
}
inline void AudioNoInterrupts() { assert(!TestAudio::locked); TestAudio::locked = true; }
inline void AudioInterrupts() { assert(TestAudio::locked); TestAudio::locked = false; }
#define AudioMemory(n) (TestAudio::poolBlocks = (n))
inline float AudioProcessorUsage() { return TestAudio::cpu; }
inline float AudioProcessorUsageMax() { return TestAudio::cpuMax; }
inline void AudioProcessorUsageMaxReset() { TestAudio::cpuMax = TestAudio::cpu; }
inline unsigned int AudioMemoryUsage() { return TestAudio::memory; }
inline unsigned int AudioMemoryUsageMax() { return TestAudio::memoryMax; }
inline void AudioMemoryUsageMaxReset() { TestAudio::memoryMax = TestAudio::memory; }

struct AudioInputI2S {};
struct AudioOutputI2S {};
struct AudioMixer4 {
    float gains[4] = {1, 1, 1, 1};
    void gain(int channel, float value) { gains[channel] = value; }
};
struct AudioFilterBiquad {
    float frequency = 0;
    void setLowpass(int, float hz, float) { frequency = hz; }
};
struct AudioEffectDelay {
    float lastDelay = 0;
    void delay(int, float ms) { lastDelay = ms; }
};
struct AudioEffectFreeverb {
    float room = 0, damp = 0;
    void roomsize(float v) { room = v; }
    void damping(float v) { damp = v; }
};
constexpr int AUDIO_INPUT_LINEIN = 1;
struct AudioControlSGTL5000 {
    float outputVolume = 0;
    int inputLevel = 0;
    void enable() {}
    void inputSelect(int) {}
    void lineInLevel(int level) { inputLevel = level; }
    void volume(float v) { outputVolume = v; }
};
struct AudioConnection {
    template<class A, class B> AudioConnection(A &, int, B &, int) {}
};
