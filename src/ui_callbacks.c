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
        FreeGameState();
        InitGameState();
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
    return render.resScale;
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
        render.resScale = setValue;
    }
    InitRenderTexture();
    game.camera.offset = (Vector2){ render.renderTexWidth/2, render.renderTexHeight/2 };
    game.camera.zoom = render.renderTexHeight/VIRTUAL_HEIGHT;
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
