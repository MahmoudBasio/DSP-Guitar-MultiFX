#ifndef CORE_CONSTANTS_H
#define CORE_CONSTANTS_H

// DSP Routing & Gain Constants
constexpr float CHORUS_DRY_GAIN         = 0.6f;
constexpr float REVERB_FROM_DELAY_GAIN  = 0.6f;
constexpr float REVERB_FROM_CHORUS_GAIN = 0.5f;
constexpr float FINAL_DRY_GAIN          = 0.65f;
constexpr float FINAL_WET_GAIN          = 0.75f;

// Filter Constants
constexpr float CHORUS_WET_FILTER_HZ           = 3200.0f;
constexpr float REPEAT_FILTER_HZ        = 1800.0f;

// Hardware Audio Constants
constexpr float OUTPUT_VOLUME           = 0.7f;
constexpr int   LINEIN_LEVEL            = 9;

// Delay Modulation Base Constants
constexpr float DELAY_MOD_DEPTH_MS      = 3.0f;
constexpr float DELAY_MOD_RATE_HZ       = 0.18f;

// UI & Hardware Timing Constants
constexpr unsigned long DEBOUNCE_DELAY_MS   = 250;
constexpr unsigned long DOUBLE_CLICK_GAP_MS = 400;

#endif // CORE_CONSTANTS_H