#include "platform/sdl_platform.h"

#include <SDL.h>

#include <stddef.h>

bool stim_sdl_platform_init(stim_sdl_platform_t * platform,
                            uint32_t width,
                            uint32_t height,
                            const char * title)
{
    lv_group_t * group;

    if((platform == NULL) || (title == NULL)) {
        return false;
    }

    SDL_SetMainReady();
    lv_init();

    platform->display = lv_sdl_window_create((int32_t)width, (int32_t)height);
    if(platform->display == NULL) {
        return false;
    }

    platform->width = width;
    platform->height = height;
    lv_display_set_default(platform->display);
    lv_sdl_window_set_resizeable(platform->display, false);
    lv_sdl_window_set_title(platform->display, title);

    group = lv_group_create();
    lv_group_set_default(group);

    platform->mouse = lv_sdl_mouse_create();
    lv_indev_set_display(platform->mouse, platform->display);
    lv_indev_set_group(platform->mouse, group);

    lv_indev_t * wheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(wheel, platform->display);
    lv_indev_set_group(wheel, group);

    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, platform->display);
    lv_indev_set_group(keyboard, group);

    return true;
}

void stim_sdl_platform_run_once(void)
{
    uint32_t wait_ms = lv_timer_handler();

    if((wait_ms == LV_NO_TIMER_READY) || (wait_ms > 16U)) {
        wait_ms = 16U;
    }
    SDL_Delay(wait_ms);
}

bool stim_sdl_platform_save_bmp(const stim_sdl_platform_t * platform, const char * path)
{
    lv_draw_buf_t * snapshot;
    SDL_Surface * surface;
    bool success = false;

    if((platform == NULL) || (platform->display == NULL) || (path == NULL)) {
        return false;
    }

    lv_refr_now(platform->display);
    snapshot = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);
    if(snapshot == NULL) {
        return false;
    }

    surface = SDL_CreateRGBSurfaceWithFormatFrom(snapshot->data,
                                                 (int)snapshot->header.w,
                                                 (int)snapshot->header.h,
                                                 32,
                                                 (int)snapshot->header.stride,
                                                 SDL_PIXELFORMAT_ARGB8888);
    if(surface == NULL) {
        lv_draw_buf_destroy(snapshot);
        return false;
    }

    success = (SDL_SaveBMP(surface, path) == 0);

    SDL_FreeSurface(surface);
    lv_draw_buf_destroy(snapshot);
    return success;
}

void stim_sdl_platform_shutdown(void)
{
    lv_deinit();
}
