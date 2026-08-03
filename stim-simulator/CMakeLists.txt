cmake_minimum_required(VERSION 3.24)

project(stim_simulator VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(STIM_BUILD_TESTS "Build simulator model tests" ON)

# SDL is built from source so Windows and macOS use the same dependency path.
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.32.10
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sdl2)

# Pin the released LVGL version. Demos/examples and heavyweight renderers are
# intentionally disabled to keep simulator build time and binary size bounded.
set(LV_BUILD_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h" CACHE PATH "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG v9.5.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(lvgl)

target_link_libraries(lvgl PUBLIC SDL2::SDL2-static)

add_library(stim_model STATIC
    src/model/stim_model.c
)
target_include_directories(stim_model PUBLIC src)

add_executable(stim-simulator
    src/main.c
    src/platform/sdl_platform.c
    src/ui/stim_ui.c
    assets/fonts/stim_font_16.c
    assets/fonts/stim_font_20.c
    assets/fonts/stim_font_24.c
)
target_include_directories(stim-simulator PRIVATE src assets/fonts)
target_compile_definitions(stim-simulator PRIVATE SDL_MAIN_HANDLED)
target_link_libraries(stim-simulator PRIVATE stim_model lvgl SDL2::SDL2-static)

if(MSVC)
    target_compile_options(stim_model PRIVATE /utf-8 /W4 /WX)
    target_compile_options(stim-simulator PRIVATE /utf-8 /W4 /WX)
    target_link_options(stim-simulator PRIVATE /SUBSYSTEM:CONSOLE)
else()
    target_compile_options(stim_model PRIVATE -Wall -Wextra -Wpedantic -Werror)
    target_compile_options(stim-simulator PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(STIM_BUILD_TESTS)
    enable_testing()
    add_executable(stim-model-tests tests/test_model.c)
    target_link_libraries(stim-model-tests PRIVATE stim_model)
    if(MSVC)
        target_compile_options(stim-model-tests PRIVATE /utf-8 /W4 /WX)
    else()
        target_compile_options(stim-model-tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
    add_test(NAME stim-model-tests COMMAND stim-model-tests)
endif()

