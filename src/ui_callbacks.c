// EXPLANATION:
// Callback functions for user interface buttons
// See the main ui.h file for more info

// Title
void UiCallbackStartGame(void)
{
    game.currentScreen = SCREEN_GAMEPLAY;
    ChangeUiMenu(UI_MENU_NONE);
}

void UiCallbackSettings(void)
{
    ChangeUiMenu(UI_MENU_SETTINGS);
}

void UiCallbackExit(void)
{
    game.gameShouldExit = true;
}

// Paused
void UiCallbackResume(void)
{
    ChangeUiMenu(UI_MENU_NONE);
    game.paused = false;
    game.currentScreen = SCREEN_GAMEPLAY;
    game.resumeInputCooldown = true;
}

void UiCallbackGoBack(void)
{
    if (game.currentScreen == SCREEN_TITLE)
        ChangeUiMenu(UI_MENU_TITLE);

    if (game.currentScreen == SCREEN_GAMEPLAY)
        ChangeUiMenu(UI_MENU_PAUSE);
}

void UiCallbackGoToTitle(void)
{
    ChangeUiMenu(UI_MENU_TITLE);

    // Reset game state if returning from gameplay
    if (game.currentScreen == SCREEN_GAMEPLAY)
        InitGameState(SCREEN_TITLE);
}

// Settings
bool UiCallbackCheckFullscreen(void)
{
    return game.fullscreen;
}

void UiCallbackToggleFullscreen(void)
{
    ToggleBorderlessWindowed();
    input.cancelTime = 0.25f;
}

void UiCallbackSetVolume(float setValue, float min, float max)
{
    const float playCooldown = 0.1f;
    setValue = Clamp(setValue, min, max);
    SetMasterVolume(setValue);
    if (ui.playbackTimer < EPSILON)
    {
        ui.playbackTimer = playCooldown;
        PlaySound(ui.sounds.menu);
    }
}

// bool UiCallbackCheckShader(void)
// {
//     return render.shaderEnabled;
// }

// void UiCallbackToggleShader(void)
// {
//     render.shaderEnabled = !render.shaderEnabled;
// }

// float UiCallbackGetPixelSize(void)
// {
//     return render.pixelSize;
// }

// void UiCallbackSetPixelSize(float setValue, float min, float max)
// {
//     setValue = Clamp(setValue, min, max);
//     render.pixelSize = (int)setValue;
//     SetShaderValue(render.shader, render.shaderPixelWidthLoc, &render.pixelSize, SHADER_UNIFORM_FLOAT);
//     SetShaderValue(render.shader, render.shaderPixelHeightLoc, &render.pixelSize, SHADER_UNIFORM_FLOAT);
// }
