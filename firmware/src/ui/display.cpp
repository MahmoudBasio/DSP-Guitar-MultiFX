#include "ui/display.h"
#include "ui/ui.h"
#include "core/hardware.h"
#include "effects/effect_manager.h"
#include "dsp/parameters.h"
#include <stdio.h>

static const char* parameterName(int effect, int parameter) {
    return effect == 0 ? delay_params[parameter] :
           effect == 1 ? reverb_params[parameter] : chorus_params[parameter];
}

static void formatValue(char* out, size_t size, int effect, int parameter) {
    if (effect == 0 && parameter == 0)
        snprintf(out, size, "%d ms", currentDelayParams.time_ms);
    else if (effect == 0 && parameter == 1)
        snprintf(out, size, "%.0f%%", currentDelayParams.feedback * 100.0f);
    else if (effect == 2 && parameter == 0)
        snprintf(out, size, "%.2f Hz", currentChorusParams.rate_hz);
    else if (effect == 2 && parameter == 1)
        snprintf(out, size, "%.1f ms", currentChorusParams.depth);
    else if (effect == 2 && parameter == 2)
        snprintf(out, size, "%.1f ms", currentChorusParams.base_ms);
    else if (effect == 2 && parameter == 4)
        snprintf(out, size, "%s", currentChorusParams.wet_filter ? "ON" : "OFF");
    else snprintf(out, size, "%d%%", values[effect][parameter]);
}

void drawUI() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tr);
    if (current_screen == MAIN_MENU) {
        bool any = EffectManager::delay.isEnabled() || EffectManager::reverb.isEnabled() || EffectManager::chorus.isEnabled();
        u8g2.drawStr(0, 10, any ? "EFFECTS" : "FX OFF");
        u8g2.drawHLine(0, 13, 128);
        for (int i = 0; i < 4; ++i) {
            char line[24];
            bool on = i == 0 ? EffectManager::delay.isEnabled() : i == 1 ? EffectManager::reverb.isEnabled() : EffectManager::chorus.isEnabled();
            if (i < 3) snprintf(line, sizeof(line), "%s [%s]", main_menu[i], on ? "ON" : "OFF");
            else snprintf(line, sizeof(line), "%s", main_menu[i]);
            int y = 26 + i * 12;
            if (i == main_index) u8g2.drawStr(0, y, ">");
            u8g2.drawStr(10, y, line);
        }
    } else {
        u8g2.drawStr(0, 10, main_menu[main_index]);
        u8g2.drawHLine(0, 13, 128);
        if (current_screen == SUB_MENU) {
            // Keep the selected item visible in a three-row viewport.
            int first = sub_index < 3 ? 0 : sub_index - 2;
            for (int row = 0; row < 3 && first + row < parameterCount(main_index); ++row) {
                int p = first + row;
                char value[20], line[32];
                formatValue(value, sizeof(value), main_index, p);
                snprintf(line, sizeof(line), "%s: %s", parameterName(main_index, p), value);
                int y = 27 + row * 12;
                if (p == sub_index) u8g2.drawStr(0, y, ">");
                u8g2.drawStr(8, y, line);
            }
        } else {
            char value[20];
            formatValue(value, sizeof(value), main_index, sub_index);
            u8g2.drawStr(0, 28, parameterName(main_index, sub_index));
            u8g2.setFont(u8g2_font_9x15_tr);
            u8g2.drawStr(0, 48, value);
        }
        u8g2.setFont(u8g2_font_u8glib_4_tf);
        u8g2.drawStr(0, 63, "2x CLICK: BACK");
    }
    u8g2.sendBuffer();
}
