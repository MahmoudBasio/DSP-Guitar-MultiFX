#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

extern volatile bool system_is_on;
void processFootswitchEvents();

void initSystem();

// Task Scheduler Callbacks
void cb_SystemCheck();

// Hardware Interrupt Service Routines (ISRs)
void isrToggleReverb();
void isrToggleDelay();
void isrToggleChorus();

#endif // CORE_SYSTEM_H