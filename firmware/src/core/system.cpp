#include "core/system.h"
#include "core/hardware.h"
#include "config/pins.h"
#include "config/constants.h"
#include "audio/audio_manager.h"
#include "effects/effect_manager.h"
#include "ui/ui.h"


namespace {
volatile bool pending[3] = {false, false, false};
unsigned long lastPress[3] = {0, 0, 0};
bool seenPress[3] = {false, false, false};
void recordPress(int index) {
    unsigned long now = millis();
    if (!seenPress[index] || now - lastPress[index] >= DEBOUNCE_DELAY_MS) {
        lastPress[index] = now;
        seenPress[index] = true;
        pending[index] = !pending[index];
    }
}
}

void initSystem() {
    attachInterrupt(digitalPinToInterrupt(PIN_FS_REVERB), isrToggleReverb, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_FS_DELAY), isrToggleDelay, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_FS_CHORUS), isrToggleChorus, FALLING);
}
void isrToggleReverb() { recordPress(0); }
void isrToggleDelay() { recordPress(1); }
void isrToggleChorus() { recordPress(2); }

void processFootswitchEvents() {
    bool events[3];
    noInterrupts();
    for (int i = 0; i < 3; ++i) { events[i] = pending[i]; pending[i] = false; }
    interrupts();
    if (events[0]) EffectManager::toggleReverb();
    if (events[1]) EffectManager::toggleDelay();
    if (events[2]) EffectManager::toggleChorus();
}

void cb_SystemCheck() {
    static bool last_system_state = true;
    bool current_system_state = (digitalRead(PIN_SYS_BTN) == LOW); 

    if (current_system_state != last_system_state) {
        last_system_state = current_system_state;
        if (current_system_state) {
            system_is_on = true;
            digitalWrite(PIN_SYS_LED, HIGH);
            AudioManager::setSystemVolume(OUTPUT_VOLUME); 
            ui_needs_update = true;            
        } else {
            system_is_on = false;
            digitalWrite(PIN_SYS_LED, LOW);
            AudioManager::setSystemVolume(0);             
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_7x14_tr);
            u8g2.drawStr(20, 38, "SYSTEM STANDBY");
            u8g2.sendBuffer();
        }
    }
}