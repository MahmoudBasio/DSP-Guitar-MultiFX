#ifndef CORE_CONSTANTS_H
#define CORE_CONSTANTS_H

// DSP Routing & Gain Constants
// The default Teensy Audio Library path is signed 16-bit, 44.1 kHz / 128 samples.
constexpr float DSP_SAMPLE_RATE_HZ      = 44100.0f;
constexpr int   DSP_BLOCK_SAMPLES       = 128;
constexpr int   AUDIO_MEMORY_BLOCKS     = 400;
constexpr int   AUDIO_GRAPH_RESERVE_BLOCKS = 32;
constexpr int   CHORUS_BUFFER_SAMPLES   = 2048;
constexpr float CHORUS_MIN_BASE_MS      = 10.0f;
constexpr float CHORUS_MAX_BASE_MS      = 30.0f;
constexpr float CHORUS_MAX_DEPTH_MS     = 30.0f; // Further limited by base/buffer.
constexpr float DELAY_MIN_MS            = 10.0f;
constexpr float DELAY_MAX_MS            = 1000.0f;
constexpr float DELAY_MAX_FEEDBACK      = 0.95f;
constexpr float CHORUS_DRY_GAIN         = 0.6f;
constexpr float REVERB_FROM_DELAY_GAIN  = 0.6f;
constexpr float REVERB_FROM_CHORUS_GAIN = 0.5f;
constexpr float FINAL_DRY_GAIN          = 0.65f;
constexpr float FINAL_WET_GAIN          = 0.75f;

// Filter Constants
constexpr float PRE_FILTER_HZ           = 3200.0f;
constexpr float REPEAT_FILTER_HZ        = 1800.0f;

// Hardware Audio Constants
constexpr float OUTPUT_VOLUME           = 0.7f;
constexpr int   LINEIN_LEVEL            = 9;

// Delay Modulation Base Constants
constexpr float DELAY_MOD_DEPTH_MS      = 3.0f;
constexpr float DELAY_MOD_RATE_HZ       = 0.18f;

// UI & Hardware Timing Constants
constexpr unsigned long DEBOUNCE_DELAY_MS   = 30;
constexpr unsigned long DOUBLE_CLICK_GAP_MS = 400;

#endif // CORE_CONSTANTS_H
