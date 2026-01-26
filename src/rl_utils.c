// EXPLANATION:
// Utilities for making raylib even nicer to use
// See header for more documentation/descriptions

// Asset manager
// - track assets in a list, and then free all assets in that list
// ----------------------------------------------------------------------------
Texture LoadTextureAssetEx(RaylibAssets *pool, char* fileName, TextureFilter filter)
{
    Texture t = LoadTexture(fileName);
    SetTextureFilter(t, filter);
    arrput(pool->textures, t);
    return t;
}

Texture LoadTextureAsset(RaylibAssets *pool, char* fileName)
{
    TextureFilter defaultFilter = TEXTURE_FILTER_BILINEAR;
    return LoadTextureAssetEx(pool, fileName, defaultFilter);
}

Sound LoadSoundAsset(RaylibAssets *pool, const char *fileName, float volume)
{
    Sound s = LoadSound(fileName);
    SetSoundVolume(s, volume);
    arrput(pool->sounds, s);
    return s;
}

Music LoadMusicAsset(RaylibAssets *pool, const char *fileName, float volume)
{
    Music m = LoadMusicStream(fileName);
    SetMusicVolume(m, volume);
    arrput(pool->music, m);
    return m;
}

void FreeRaylibAssets(RaylibAssets *pool)
{
    for (int i = 0; i < arrlen(pool->textures); i++)
        UnloadTexture(pool->textures[i]);

    for (int i = 0; i < arrlen(pool->sounds); i++)
        UnloadSound(pool->sounds[i]);

    for (int i = 0; i < arrlen(pool->music); i++)
        UnloadMusicStream(pool->music[i]);

    arrfree(pool->textures);
    arrfree(pool->sounds);
    arrfree(pool->music);
}

// Draw sprites
// ----------------------------------------------------------------------------
void DrawSpriteOnRectangle(Texture *sprite, Rectangle src, Rectangle rect, float angle)
{
    // prevent bordering sprites in the atlas from bleeding over
    src.x += 0.05f;
    src.y += 0.05f;
    src.width -= 0.1f;
    src.height -= 0.1f;
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

    // prevent bordering sprites in the atlas from bleeding over
    src.x += 0.05f;
    src.y += 0.05f;
    src.width -= 0.1f;
    src.height -= 0.1f;

    DrawTexturePro(*sprite, src, spriteDest, spriteOrigin, angle, WHITE);
}
