#include "core/scheduler.h"
#include "core/system.h"
#include "ui/encoder.h"
#include "ui/display.h"
#include "ui/ui.h"
#include "effects/effect_manager.h"
#include "config/constants.h"
#include <Arduino.h>

TSScheduler sched;

void task_EncoderPoll() { pollEncoder(); }

void task_DelayModulation() {
    if (!system_is_on) return;
    if (EffectManager::delay.isEnabled()) {
        float t = millis() / 1000.0f;
        float mod = DELAY_MOD_DEPTH_MS * sinf(2.0f * PI * DELAY_MOD_RATE_HZ * t);
        EffectManager::delay.updateModulation(mod);
    }
}

void task_UIRefresh() {
    if (!system_is_on) return;
    if (ui_needs_update) {
        drawUI();
        ui_needs_update = false;
    }
}

// Map the callbacks to the standard task objects
TSTask tSystemCheck("SysCheck", 50, cb_SystemCheck);
TSTask tFootswitchPoll("Footswitches", 5, pollFootswitches);
TSTask tEncoderPoll("Encoder", 5, task_EncoderPoll);
TSTask tDelayMod("Modulation", 20, task_DelayModulation);
TSTask tUIRefresh("UI", 33, task_UIRefresh);

void initScheduler() {
    sched.addTask(&tSystemCheck);
    sched.addTask(&tFootswitchPoll);
    sched.addTask(&tEncoderPoll);
    sched.addTask(&tDelayMod);
    sched.addTask(&tUIRefresh);
    sched.enableAll();
}
