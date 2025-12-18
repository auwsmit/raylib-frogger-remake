// EXPLANATION:
// The main entry point for the game/program
// See header files for overall layout and explanations

#define uint unsigned int

#include "common.h" // all project header includes

// Platform layer
#if defined(PLATFORM_WEB)
    #include "platform_web.c"
#elif defined(PLATFORM_DESKTOP)
    #include "platform_desktop.c"
#endif

#include "rl_utils.c" // raylib convenience

// Modules
#include "render.c"
#include "input.c"
#include "logo.c"
#include "ui_callbacks.c"
#include "ui.c"

// Game code
#include "frogger.c"

// Globals
GameState  game;
InputState input;
UiState    ui;
RenderData render;

// Local Functions Declaration
void UpdateDrawFrame(void); // main game loop

// Main entry point
int main(void)
{
    // Initialization
    // ----------------------------------------------------------------------------

    // New window
    uint windowFlags = FLAG_MSAA_4X_HINT;
    windowFlags |= PlatformWindowFlags();
    SetConfigFlags(windowFlags);
    InitWindow(INITIAL_WIDTH, INITIAL_HEIGHT, WINDOW_TITLE);
    SetWindowMinSize(320, 240);
    InitAudioDevice();

    InitWindowRender();
    InitRaylibLogo();
    InitUiState();
    InitGameState();
    InitDefaultInputSettings();

    // Debug exit:
    // SetExitKey(KEY_NULL);
    SetExitKey(KEY_Q);
    SetMasterVolume(0.15f);

    // Start game loop
    PlatformRunGameLoop(UpdateDrawFrame);

    // De-Initialization
    // ----------------------------------------------------------------------------
    FreeGameState();
    FreeUiState();
    CloseAudioDevice();
    UnloadShader(render.shader);
    UnloadRenderTexture(render.renderTarget);
    CloseWindow(); // close window and OpenGL context

    return 0;
}

// Update game data and draw elements to the screen for the current frame
void UpdateDrawFrame(void)
{
    // Update
    // ----------------------------------------------------------------------------

    UpdateInputFrame();

    // Global input checks
    if (input.global.fullscreen)
    {
        // Borderless Windowed is generally nicer to use on desktop
        ToggleBorderlessWindowed();
        CancelInputActions(); // clear input until a new press
    }

    // Debug:
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_S))
    {
        render.shaderEnabled = !render.shaderEnabled;
        CancelInputActions();
    }

    // Global updates
    game.isFullscreen = (IsWindowFullscreen() ||
                       IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE));
    game.frameTime = GetFrameTime();
    game.frameCount++;

    // Update window data for proper aspect ratio, cameras, and shaders
    UpdateWindowRenderFrame();

    // Update for current screen
    switch(game.currentScreen)
    {
        case SCREEN_LOGO:     UpdateRaylibLogo();
                              break;
        case SCREEN_TITLE:    UpdateUiFrame();
                              break;
        case SCREEN_GAMEPLAY: UpdateGameFrame();
                              break;
        default: break;
    }

    // Draw
    // ----------------------------------------------------------------------------

    // Draw to render texture
    BeginTextureMode(render.renderTarget);
        BeginMode2D(game.camera);

            switch(game.currentScreen)
            {
                case SCREEN_LOGO:     DrawRaylibLogo();
                                      break;
                case SCREEN_TITLE:    ClearBackground(DARKGREEN);
                                      break;
                case SCREEN_GAMEPLAY: DrawGameFrame();
                                      break;
                default: break;
            }

        EndMode2D();
    EndTextureMode();

    // Draw render texture
    BeginDrawing();
        ClearBackground(BLACK);
        // Draw full-screen shader effect
        if (render.shaderEnabled) BeginShaderMode(render.shader);

            DrawTexturePro(render.renderTarget.texture,
                           (Rectangle){ 0, 0,
                           (float)render.renderTarget.texture.width,
                           (float)-render.renderTarget.texture.height },
                           (Rectangle){ render.x, render.y,
                           render.width, render.height },
                           Vector2Zero(), 0, WHITE);

        if (render.shaderEnabled) EndShaderMode();

        // Draw UI above render texture
        BeginScissorMode((int)render.x, // draw within aspect ratio
                         (int)render.y, (int)render.width, (int)render.height);
        BeginMode2D(ui.camera);

            switch(game.currentScreen)
            {
                case SCREEN_TITLE:
                case SCREEN_GAMEPLAY: DrawUiFrame();
                                      break;
                default: break;
            }
        EndMode2D();
        EndScissorMode();

        // Draw touch screen gamepad on screen edges
        DrawUiGamepad();

    EndDrawing();
}

