#include "config/presets.h"

const int preset_values[3][5] = {
    {42, 61, 55}, // Delay (Time, Feedback, Wet)
    {90, 30, 60}, // Reverb (Size, Damping, Wet)
    {4,  63, 50, 80, 0}  // Chorus (Rate, Depth, Base, Wet, Wet filter); depth stays below base
};