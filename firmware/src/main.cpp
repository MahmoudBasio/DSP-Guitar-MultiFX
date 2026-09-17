#include <Arduino.h>
#include "core/hardware.h"
#include "core/system.h"
#include "core/scheduler.h"
#include "audio/audio_manager.h"
#include "effects/effect_manager.h"
#include "ui/display.h"
#include "config/constants.h"
#include "core/diagnostics.h"

void setup() {
    // 1. Start Serial for debugging
    Serial.begin(115200);

    // 2. Initialize low-level hardware (Pins, Encoder, Display)
    initHardware();
    
    // 3. Initialize Audio Graph (Memory, I2S, Static Patch Cords)
    AudioManager::init();

    // 4. Initialize Effect wrappers and set default states
    EffectManager::init();

    // 5. Honor the physical master switch before unmuting the output.
    initSystem();
    if (system_is_on) drawUI();

    // 6. Register and enable foreground control tasks
    initScheduler();
    Diagnostics::init();
}

void loop() {
    // Cooperative control tasks run here; AudioStream runs in the audio interrupt.
    sched.execute(); 
    Diagnostics::poll();
}
