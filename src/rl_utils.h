// EXPLANATION:
// Utilities for making raylib even nicer to use

#ifndef FROGGER_RL_UTIL_HEADER_GUARD
#define FROGGER_RL_UTIL_HEADER_GUARD

// Types and Structures
// ----------------------------------------------------------------------------
typedef struct { // keep track of raylib resources to free together
    Texture *textures;
    Sound *sounds;
} RaylibAssets;

// Prototypes
// ----------------------------------------------------------------------------

// Asset manager
Texture LoadTextureAsset(RaylibAssets *pool, char* fileName);
Sound LoadSoundAsset(RaylibAssets *pool, const char *fileName);
void FreeRaylibAssets(RaylibAssets *pool);

// Draw sprites
void DrawSpriteOnRectangle(Texture *sprite, Rectangle src, Rectangle rect, float angle); // Draw a sprite on a rectangle
void DrawSpriteOnCircle(Texture *sprite, Rectangle src, // Draw a sprite centered on a circle
                        Vector2 center, float radius, float angle);

#endif // FROGGER_RL_UTIL_HEADER_GUARD
