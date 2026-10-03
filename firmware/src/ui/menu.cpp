#include "ui/menu.h"
#include "ui/ui.h"
#include "effects/effect_manager.h"
#include <Arduino.h>

void handleButtonLogic() {
    if (current_screen == MAIN_MENU) {
        if (main_index == 3) {
            EffectManager::resetToDefaults();
        } else {
            current_screen = SUB_MENU;
            sub_index = 0;
        }
    } 
    else if (current_screen == SUB_MENU) {
        current_screen = EDIT_VALUE;
    } 
    else if (current_screen == EDIT_VALUE) {
        current_screen = SUB_MENU;
    }
}

void handleRotation(int dir) {
    if (current_screen == MAIN_MENU) {
        main_index = constrain(main_index + dir, 0, 3);
    } else if (current_screen == SUB_MENU) {
        sub_index = constrain(sub_index + dir, 0, parameterCount(main_index) - 1);
    } else if (current_screen == EDIT_VALUE) {
        values[main_index][sub_index] = constrain(values[main_index][sub_index] + dir, 0, (main_index == 2 && sub_index == 4) ? 1 : 100);
        EffectManager::applySettings(); 
    }
}