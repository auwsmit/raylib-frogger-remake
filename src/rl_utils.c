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
void DrawSpriteRectangle(Texture *sprite, Rectangle rect, float angle)
{
    Rectangle src = { 0, 0, (float)sprite->width, (float)sprite->height };
    DrawTexturePro(*sprite, src, rect, Vector2Zero(), angle, WHITE);
}

void DrawSpriteCircle(Texture *sprite, Vector2 center, float radius,
                      float angle, float textureScale)
{
    float spriteScale = radius*2.0f/sprite->width*textureScale;
    Rectangle spriteSrc = { 0.0f, 0.0f, (float)sprite->width, (float)sprite->height };
    Rectangle spriteDest = {
        center.x, center.y,
        sprite->width*spriteScale, sprite->height*spriteScale
    };
    Vector2 spriteOrigin = {
        sprite->width/2*spriteScale,
        sprite->height/2*spriteScale };

    DrawTexturePro(*sprite, spriteSrc, spriteDest, spriteOrigin, angle, WHITE);
}
