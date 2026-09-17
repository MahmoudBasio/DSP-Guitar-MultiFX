#include "ui/display.h"
#include "ui/ui.h"
#include "core/hardware.h"
#include "effects/effect_manager.h"
#include "dsp/parameters.h"
#include <stdio.h>
#include <string.h>

namespace {
void formatParameter(char *text, size_t size, int effect, int parameter) {
    if (effect == 0) {
        if (parameter == 0) snprintf(text, size, "%d ms", currentDelayParams.time_ms);
        else snprintf(text, size, "%.0f%%", 100.0f * (parameter == 1 ?
            currentDelayParams.feedback : currentDelayParams.wet));
    } else if (effect == 1) {
        const float value = parameter == 0 ? currentReverbParams.room_size :
            (parameter == 1 ? currentReverbParams.damping : currentReverbParams.wet);
        snprintf(text, size, "%.0f%%", value * 100.0f);
    } else {
        const float value = parameter == 0 ? currentChorusParams.rate_hz :
            (parameter == 1 ? currentChorusParams.depth : currentChorusParams.base_ms);
        snprintf(text, size, "%.2f %s", value, parameter == 0 ? "Hz" : "ms");
    }
}
}

void drawUI() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tr);
    
    if (current_screen == MAIN_MENU) {
        u8g2.drawStr(15, 10, "--- MAIN MENU ---");
        u8g2.drawHLine(0, 13, 128);
        for (int i = 0; i < 4; i++) {
            int y = 28 + (i * 11);
            
            char itemText[30];
            if (i == 0) sprintf(itemText, "DELAY  [%s]", EffectManager::delay.isEnabled() ? "ON" : "OFF");
            else if (i == 1) sprintf(itemText, "REVERB [%s]", EffectManager::reverb.isEnabled() ? "ON" : "OFF");
            else if (i == 2) sprintf(itemText, "CHORUS [%s]", EffectManager::chorus.isEnabled() ? "ON" : "OFF");
            else strcpy(itemText, main_menu[i]); 

            if (i == main_index) { 
                u8g2.drawStr(5, y, ">"); 
                u8g2.drawStr(20, y, itemText); 
            } else { 
                u8g2.drawStr(15, y, itemText); 
            }
        }
    } 
    else {
        if (current_screen == SUB_MENU) {
            char title[30]; sprintf(title, "%s SETTINGS", main_menu[main_index]);
            u8g2.drawStr(10, 10, title); u8g2.drawHLine(0, 13, 128);
            for (int i = 0; i < 3; i++) {
                int y = 30 + (i * 11);
                const char* pName = (main_index == 0) ? delay_params[i] : (main_index == 1) ? reverb_params[i] : chorus_params[i];
                
                char lineBuf[30];
                char valueText[20];
                formatParameter(valueText, sizeof(valueText), main_index, i);
                snprintf(lineBuf, sizeof(lineBuf), "%s: %s", pName, valueText);

                if (i == sub_index) { 
                    u8g2.drawStr(5, y, ">"); 
                    u8g2.drawStr(15, y, lineBuf); 
                } else { 
                    u8g2.drawStr(15, y, lineBuf); 
                }
            }
        } 
        else if (current_screen == EDIT_VALUE) {
            u8g2.drawStr(5, 10, "ADJUSTING...");
            const char* pName = (main_index == 0) ? delay_params[sub_index] : (main_index == 1) ? reverb_params[sub_index] : chorus_params[sub_index];
            u8g2.setFont(u8g2_font_9x15_tr); u8g2.drawStr(10, 30, pName);
            
            u8g2.drawFrame(10, 38, 108, 10); 
            u8g2.drawBox(12, 40, values[main_index][sub_index], 6);
            
            char s[20];
            formatParameter(s, sizeof(s), main_index, sub_index);
            u8g2.setFont(u8g2_font_6x12_tr); u8g2.drawStr(15, 60, s); 
        }
        
        u8g2.setFont(u8g2_font_u8glib_4_tf);
        u8g2.drawStr(60, 62, "2x CLICK: BACK");
    }
    u8g2.sendBuffer();
}
