#include "model/stim_model.h"
#include "platform/sdl_platform.h"
#include "ui/stim_ui.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STIM_SCREEN_WIDTH 1280U
#define STIM_SCREEN_HEIGHT 800U
#define STIM_SCREENSHOT_SETTLE_FRAMES 45U

typedef struct {
    stim_screen_t initial_screen;
    const char * screenshot_path;
} app_options_t;

static bool parse_options(int argc, char ** argv, app_options_t * options)
{
    int index;

    options->initial_screen = STIM_SCREEN_MEDIUM;
    options->screenshot_path = NULL;

    for(index = 1; index < argc; ++index) {
        if((strcmp(argv[index], "--screen") == 0) && ((index + 1) < argc)) {
            ++index;
            if(strcmp(argv[index], "medium") == 0) {
                options->initial_screen = STIM_SCREEN_MEDIUM;
            }
            else if(strcmp(argv[index], "low") == 0) {
                options->initial_screen = STIM_SCREEN_LOW;
            }
            else {
                fprintf(stderr, "Unknown screen: %s\n", argv[index]);
                return false;
            }
        }
        else if((strcmp(argv[index], "--screenshot") == 0) && ((index + 1) < argc)) {
            options->screenshot_path = argv[++index];
        }
        else if(strcmp(argv[index], "--help") == 0) {
            printf("Usage: stim-simulator [--screen medium|low] [--screenshot output.bmp]\n");
            return false;
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[index]);
            return false;
        }
    }
    return true;
}

int main(int argc, char ** argv)
{
    stim_sdl_platform_t platform = {0};
    stim_model_t model;
    app_options_t options;

    if(!parse_options(argc, argv, &options)) {
        return 2;
    }

    if(!stim_sdl_platform_init(&platform,
                               STIM_SCREEN_WIDTH,
                               STIM_SCREEN_HEIGHT,
                               "STIM 低中频治疗仪 UI Simulator")) {
        fputs("Failed to initialize SDL/LVGL platform.\n", stderr);
        return 1;
    }

    stim_model_init(&model);
    stim_ui_create(&model);
    stim_ui_show_screen(options.initial_screen);

    if(options.screenshot_path != NULL) {
        uint32_t frame;
        for(frame = 0U; frame < STIM_SCREENSHOT_SETTLE_FRAMES; ++frame) {
            stim_sdl_platform_run_once();
        }
        if(!stim_sdl_platform_save_bmp(&platform, options.screenshot_path)) {
            fputs("Failed to save screenshot.\n", stderr);
            stim_sdl_platform_shutdown();
            return 1;
        }
        stim_sdl_platform_shutdown();
        return 0;
    }

    for(;;) {
        stim_sdl_platform_run_once();
    }
}

