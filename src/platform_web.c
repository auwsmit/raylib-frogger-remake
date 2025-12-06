// EXPLANATION:
// Contains code which is specific to the WEB platform

#include <emscripten/emscripten.h>

#define GLSL_VERSION 100
#define PLATFORM_TITLE_PADDING UI_MENU_FONT_SIZE
#define PLATFORM_HAS_ANALOG_TRIGGERS 0
#define PLATFORM_CAN_EXIT 0

uint PlatformWindowFlags(void)
{
    // No additional flags for web
    return 0;
}

void PlatformRunGameLoop(void (*UpdateDrawFrame)(void))
{
    // Let emscripten handle the framerate because setting a specific one is kinda janky
    // Generally, it will use whatever the monitor's refresh rate is
    const int fps = 0;
    const int infiniteLoop = 1;

    emscripten_set_main_loop(UpdateDrawFrame, fps, infiniteLoop);
}
