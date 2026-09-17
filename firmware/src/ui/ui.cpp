#include "ui/ui.h"

volatile Screen current_screen = MAIN_MENU;
volatile int main_index = 0; 
int sub_index = 0;  
volatile bool ui_needs_update = true;

int values[3][3];

const char* main_menu[] = {"DELAY", "REVERB", "CHORUS", "RESET ALL"};
const char* delay_params[]  = {"Time", "Feedback", "Mix"};
const char* reverb_params[] = {"Room Size", "Damping", "Wet"};
const char* chorus_params[] = {"Rate", "Depth", "Base"};
