// EXPLANATION:
// Utilities for making raylib even nicer to use
// See header for more documentation/descriptions

// Asset manager
// ----------------------------------------------------------------------------
Texture LoadTextureAsset(RaylibAssets *pool, char* fileName)
{
    Texture t = LoadTexture(fileName);
    SetTextureFilter(t, TEXTURE_FILTER_BILINEAR);
    arrput(pool->textures, t);
    return t;
}

Sound LoadSoundAsset(RaylibAssets *pool, const char *fileName)
{
    Sound s = LoadSound(fileName);
    arrput(pool->sounds, s);
    return s;
}

void FreeRaylibAssets(RaylibAssets *pool)
{
    for (int i = 0; i < arrlen(pool->textures); i++)
        UnloadTexture(pool->textures[i]);

    for (int i = 0; i < arrlen(pool->sounds); i++)
        UnloadSound(pool->sounds[i]);

    arrfree(pool->textures);
    arrfree(pool->sounds);
}

// Draw sprites
// ----------------------------------------------------------------------------
void DrawSpriteOnRectangle(Texture *sprite, Rectangle src, Rectangle rect, float angle)
{
    DrawTexturePro(*sprite, src, rect, Vector2Zero(), angle, WHITE);
}

void DrawSpriteOnCircle(Texture *sprite, Rectangle src,
                        Vector2 center, float radius, float angle)
{
    Rectangle spriteDest = {
        center.x, center.y,
        radius*2, radius*2
    };
    Vector2 spriteOrigin = { radius, radius };

    DrawTexturePro(*sprite, src, spriteDest, spriteOrigin, angle, WHITE);
}
