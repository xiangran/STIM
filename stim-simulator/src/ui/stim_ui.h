#ifndef STIM_UI_H
#define STIM_UI_H

#include "model/stim_model.h"

typedef enum {
    STIM_SCREEN_MEDIUM = 0,
    STIM_SCREEN_LOW
} stim_screen_t;

void stim_ui_create(stim_model_t * model);
void stim_ui_show_screen(stim_screen_t screen);

#endif

