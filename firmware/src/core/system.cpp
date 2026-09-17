#include "core/system.h"
#include "core/debounce.h"
#include "core/hardware.h"
#include "config/pins.h"
#include "config/constants.h"
#include "audio/audio_manager.h"
#include "effects/effect_manager.h"
#include "ui/ui.h"

namespace {
DebouncedInput switches[3];
DebouncedInput masterSwitch;
const int footswitchPins[] = {PIN_FS_DELAY, PIN_FS_REVERB, PIN_FS_CHORUS};

void applySystemState(bool on) {
    system_is_on = on;
    digitalWrite(PIN_SYS_LED, on ? HIGH : LOW);
    AudioManager::setSystemVolume(on ? OUTPUT_VOLUME : 0.0f);
    if (on) {
        ui_needs_update = true;
    } else {
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_7x14_tr);
        u8g2.drawStr(20, 38, "SYSTEM STANDBY");
        u8g2.sendBuffer();
        ui_needs_update = false;
    }
}
}

// Called after audio/effect initialization. No footswitch ISR mutates audio state.
void initSystem() {
    const uint32_t now = millis();
    for (int i = 0; i < 3; ++i) {
        switches[i].begin(digitalRead(footswitchPins[i]) == LOW, now);
    }
    masterSwitch.begin(digitalRead(PIN_SYS_BTN) == LOW, now);
    applySystemState(masterSwitch.isPressed());
}

void pollFootswitches() {
    const uint32_t now = millis();
    for (int i = 0; i < 3; ++i) {
        const bool changed = switches[i].update(
            digitalRead(footswitchPins[i]) == LOW, now, DEBOUNCE_DELAY_MS);
        // Still track releases during standby; never queue a stale press.
        if (!changed || !switches[i].isPressed() || !system_is_on) continue;
        if (i == 0) EffectManager::toggleDelay();
        else if (i == 1) EffectManager::toggleReverb();
        else EffectManager::toggleChorus();
    }
}

void cb_SystemCheck() {
    if (masterSwitch.update(digitalRead(PIN_SYS_BTN) == LOW, millis(), DEBOUNCE_DELAY_MS)) {
        applySystemState(masterSwitch.isPressed());
    }
}
