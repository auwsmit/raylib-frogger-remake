// EXPLANATION:
// Contains code which is specific to the DESKTOP platform

#define GLSL_VERSION 330
#define PLATFORM_TITLE_PADDING 0.0f
#define PLATFORM_HAS_ANALOG_TRIGGERS 1
#define PLATFORM_CAN_EXIT 1

uint PlatformWindowFlags(void)
{
    uint addedWindowFlags = FLAG_WINDOW_RESIZABLE;
    if (VSYNC_ENABLED) addedWindowFlags |= FLAG_VSYNC_HINT;
    return addedWindowFlags;
}

void PlatformRunGameLoop(void (*UpdateDrawFrame)(void))
{
    if (MAX_FRAMERATE > 0)
        SetTargetFPS(MAX_FRAMERATE);

    // Main game loop
    while (!WindowShouldClose() && !game.shouldExit)
        UpdateDrawFrame();
}
