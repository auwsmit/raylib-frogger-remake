// EXPLANATION:
// Callback functions for interactive UI elements

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
    game.shouldExit = true;
}

// Paused
void UiCallbackResume(void)
{
    ChangeUiMenu(UI_MENU_NONE);
    game.isPaused = false;
    game.currentScreen = SCREEN_GAMEPLAY;
    game.isInputDisabledFromResume = true;
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
    {
        int hiScore = game.hiScore;
        FreeGameState();
        InitGameState();
        game.hiScore = hiScore;
    }
}

// Settings
bool UiCallbackCheckFullscreen(void)
{
    return game.isFullscreen;
}

void UiCallbackToggleFullscreen(void)
{
    ToggleBorderlessWindowed();
    input.cancelTime = 0.25f;
}

void UiCallbackSetVolume(float setValue, void *slider)
{
    const float cooldownTime = 0.1f;
    UiSlider *s = (UiSlider*)slider;
    setValue = Clamp(setValue, s->min, s->max);
    SetMasterVolume(setValue);
    if (ui.actionCooldownTimer < EPSILON)
    {
        ui.actionCooldownTimer = cooldownTime;
        PlaySound(ui.sounds.menu);
    }
}

float UiCallbackGetRenderScale(void)
{
    return viewport.resScale;
}

void UiCallbackSetRenderScale(float setValue, void *slider)
{
    const float cooldownTime = 0.1f;
    UiSlider *s = (UiSlider*)slider;
    setValue = Clamp(setValue, s->min, s->max);
    setValue = roundf(setValue/s->increment)*s->increment;
    if (setValue == s->getValue()) return; // don't update if value is unchanged

    if (ui.actionCooldownTimer < EPSILON)
    {
        ui.actionCooldownTimer = cooldownTime;
        viewport.resScale = setValue;
    }
    InitRenderTexture();
    game.camera.offset = (Vector2){ viewport.renderTexWidth/2, viewport.renderTexHeight/2 };
    game.camera.zoom = viewport.renderTexHeight/VIRTUAL_HEIGHT;
}

// bool UiCallbackCheckShader(void)
// {
//     return viewport.shaderEnabled;
// }

// void UiCallbackToggleShader(void)
// {
//     viewport.shaderEnabled = !viewport.shaderEnabled;
// }

// float UiCallbackGetPixelSize(void)
// {
//     return viewport.pixelSize;
// }

// void UiCallbackSetPixelSize(float setValue, float min, float max)
// {
//     setValue = Clamp(setValue, min, max);
//     viewport.pixelSize = (int)setValue;
//     SetShaderValue(viewport.shader, viewport.shaderPixelWidthLoc, &viewport.pixelSize, SHADER_UNIFORM_FLOAT);
//     SetShaderValue(viewport.shader, viewport.shaderPixelHeightLoc, &viewport.pixelSize, SHADER_UNIFORM_FLOAT);
// }
