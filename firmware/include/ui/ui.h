#ifndef UI_UI_H
#define UI_UI_H

enum Screen { MAIN_MENU, SUB_MENU, EDIT_VALUE };

extern volatile Screen current_screen;
extern volatile int main_index;
extern int sub_index;  
extern volatile bool ui_needs_update;

extern int values[3][5];
int parameterCount(int effect);

extern const char* main_menu[];
extern const char* delay_params[];
extern const char* reverb_params[];
extern const char* chorus_params[];

#endif // UI_UI_H