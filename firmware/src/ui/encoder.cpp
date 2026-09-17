#include "ui/encoder.h"
#include "ui/ui.h"
#include "ui/menu.h"
#include "core/hardware.h"
#include "config/pins.h"
#include "core/system.h"
#include "config/constants.h"
#include <Arduino.h>

void pollEncoder() {
    if (!system_is_on) return; 

    static long oldPos = myEnc.read() / 4;
    long newPos = myEnc.read() / 4;
    if (newPos != oldPos) {
        handleRotation((newPos > oldPos) ? 1 : -1);
        oldPos = newPos;
        ui_needs_update = true;
    }

    static int lastBtn = HIGH;
    int currentBtn = digitalRead(PIN_BTN_ENC);
    
    if (currentBtn == LOW && lastBtn == HIGH) {
        unsigned long currentTime = millis();
        static unsigned long lastClickTime = 0;
        unsigned long diff = currentTime - lastClickTime;
        
        if (diff > 50) { 
            if (diff < DOUBLE_CLICK_GAP_MS) {
                current_screen = MAIN_MENU; 
            } else {
                handleButtonLogic(); 
            }
            lastClickTime = currentTime;
            ui_needs_update = true;
        }
    }
    lastBtn = currentBtn;
}
