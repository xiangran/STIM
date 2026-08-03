#ifndef STIM_SDL_PLATFORM_H
#define STIM_SDL_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef struct {
    lv_display_t * display;
    lv_indev_t * mouse;
    uint32_t width;
    uint32_t height;
} stim_sdl_platform_t;

bool stim_sdl_platform_init(stim_sdl_platform_t * platform,
                            uint32_t width,
                            uint32_t height,
                            const char * title);
void stim_sdl_platform_run_once(void);
bool stim_sdl_platform_save_bmp(const stim_sdl_platform_t * platform, const char * path);
void stim_sdl_platform_shutdown(void);

#endif

