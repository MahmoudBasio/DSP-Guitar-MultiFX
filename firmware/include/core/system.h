#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

extern volatile bool system_is_on;

void initSystem();

// Task Scheduler Callbacks
void cb_SystemCheck();

// Foreground polling; each footswitch has independent stable-state debounce.
void pollFootswitches();

#endif // CORE_SYSTEM_H
