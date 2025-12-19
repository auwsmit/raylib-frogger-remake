// EXPLANATION:
// All the game logic, including how/when to draw to screen
// See header for more documentation/descriptions

// Initialization
// ----------------------------------------------------------------------------

void InitGameState(void)
{
    game = (GameState){
        .currentScreen = SCREEN_LOGO,

        // Center camera
        .camera.target = (Vector2){ VIRTUAL_WIDTH/2, VIRTUAL_HEIGHT/2 },
        .camera.offset = (Vector2){ render.renderTexWidth/2, render.renderTexHeight/2 },
        .camera.zoom = render.renderTexHeight/VIRTUAL_HEIGHT,

        .isDebugMode = DEBUG_DEFAULT
    };

    // Set up game grid positions
    Vector2 gridOffset = {
        VIRTUAL_WIDTH/2 - GRID_UNIT*GRID_RES_X/2,
        VIRTUAL_HEIGHT/2 - GRID_UNIT*GRID_RES_Y/2
    };
    for (int row = 0; row < GRID_RES_Y; row++)
    {
        for (int col = 0; col < GRID_RES_X; col++)
        {
            int index = row*GRID_RES_X + col;
            game.grid[index].x = gridOffset.x + GRID_UNIT*col;
            game.grid[index].y = gridOffset.y + GRID_UNIT*row;
        }
    }

    game.gridStart = GetGridPosition(0, 0);

    SetTimedMessage("Game Start!", 100, 2.0f);

    // Textures
    game.textures.atlas = LoadTextureAsset(&game.assets, "assets/textures/frogger.png");
    SetTextureFilter(game.textures.atlas, TEXTURE_FILTER_POINT);
    game.textures.car    = (Rectangle){ 48,   0, 16, 16 };
    game.textures.frog   = (Rectangle){  0,   0, 16, 16 };
    game.textures.dead   = (Rectangle){  0,  48, 16, 16 };
    game.textures.turtle = (Rectangle){  0,  80, 16, 16 };
    game.textures.win    = (Rectangle){ 48,  96, 16, 16 };
    game.textures.log    = (Rectangle){ 96, 128, 16, 16 };

    // Create entities
    // ----------------------------------------------------------------------------

    // Logs and Turtles
    int spawnRow = 2;
    CreateRow(ENTITY_TYPE_WALL,   spawnRow,   ".O_OO_OO_OO_OO_O.", 0);
    CreateRow(ENTITY_TYPE_WIN,    spawnRow,   "._O__O__O__O__O_.",  0);
    CreateRow(ENTITY_TYPE_LOG,    ++spawnRow, "_OOOO_.OOOO_.OOOO", GRID_UNIT*1.1f);
    CreateRow(ENTITY_TYPE_TURTLE, ++spawnRow, "___OO_.OO_.OO_.OO", -GRID_UNIT*1.5f);
    CreateRow(ENTITY_TYPE_LOG,    ++spawnRow, "__OOOOOO__OOOOOO",  GRID_UNIT*2.5f);
    CreateRow(ENTITY_TYPE_LOG,    ++spawnRow, "___OOO__OOO__OOO",  GRID_UNIT*0.5f);
    CreateRow(ENTITY_TYPE_TURTLE, ++spawnRow, "_OOO_OOO_OOO_OOO",  -GRID_UNIT*1.5f);

    // Frog
    Entity frog = {
        .sprite = game.textures.frog,
        .spriteOffset = 32,
        .type = ENTITY_TYPE_FROG,
        .speed = BASE_SPEED*4.0f,
        .radius = GRID_UNIT*0.4f,
        .color = GREEN,
    };
    Vector2 frogSpawnPos = GetGridPosition(10, 14);
    frog.position = frogSpawnPos;
    frog.position.x += GRID_UNIT/2;
    frog.position.y += GRID_UNIT/2;
    arrpush(game.entities, frog);
    game.frog = &arrlast(game.entities);
    game.spawnPos = frog.position;

    // Cars
    spawnRow = 9;
    CreateRow(ENTITY_TYPE_CAR, spawnRow,   "________.OO___.OO", -BASE_SPEED);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "O_______________",  BASE_SPEED*0.6f);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "_______O___O___O",  -BASE_SPEED*0.6f);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "_______O___O___O",  BASE_SPEED*0.4f);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "______O___.O___.O", -BASE_SPEED*0.4f);

    // Background rectangles (river, grass, road)
    game.background.water.x = game.gridStart.x;
    game.background.water.y = game.gridStart.y;
    game.background.water.width = GRID_WIDTH;
    game.background.water.height = GRID_UNIT*8;

    game.background.grassTop.x = game.gridStart.x;
    game.background.grassTop.y = GetGridPosition(0, 8).y;
    game.background.grassTop.width = GRID_WIDTH;
    game.background.grassTop.height = GRID_UNIT;

    game.background.grassBottom.x = game.gridStart.x;
    game.background.grassBottom.y = GetGridPosition(0, 14).y;
    game.background.grassBottom.width = GRID_WIDTH;
    game.background.grassBottom.height = GRID_UNIT*2;
}

void CreateRow(EntityType type, int row, char *pattern, float speed)
{
    Color logColor = ColorBrightness(BROWN, 0.0f - (float)row*0.10f);
    float carTextureOffsets[5] = {
        32, // truck
        80, // racer 1
        64, // car
        16, // tractor
        0,  // racer 2
    };
    float carTextureSizes[5] = { 32, 16, 16, 16, 16 };
    int spriteIdx = row - 9;
    Vector2 currentPos = GetGridPosition(0, row);
    float widthExtend = 0;
    bool isExtending = false;

    for (char *c = pattern; *c != '\0'; c++)
    {
        if (*c == 'O') widthExtend = GRID_UNIT;
        if (*c == '.') currentPos.x += GRID_UNIT/2;
        if (*c == '_') currentPos.x += GRID_UNIT;

        if (*c == '_' || *c == '.') isExtending = false;
        if (isExtending)
        {
            arrlast(game.entities).rec.width += widthExtend;
            currentPos.x += widthExtend;
        }

        if (*c == '_' || *c == '.' || isExtending)
            continue;

        Entity e = { 0 };
        e.type = type;
        e.speed = speed;
        e.rec.x = currentPos.x;
        e.rec.y = currentPos.y;
        e.rec.width = widthExtend;
        currentPos.x += widthExtend;
        e.rec.height = GRID_UNIT;

        isExtending = true;
        if (type == ENTITY_TYPE_TURTLE)
        {
            e.sprite = game.textures.turtle;
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_PLATFORM;
            e.radius = GRID_UNIT/2; //TEMP until sprites
            e.position.x = e.rec.x + e.radius; //TEMP until sprites
            e.position.y = e.rec.y + e.radius;
            e.color = ColorBrightness(MAROON, -0.4f);
            isExtending = false;
        }

        if (type == ENTITY_TYPE_LOG)
        {
            e.sprite = game.textures.log;
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_PLATFORM;
            e.color = logColor;
        }

        if (type == ENTITY_TYPE_CAR)
        {
            e.sprite = game.textures.car;
            e.sprite.x += carTextureOffsets[spriteIdx];
            e.sprite.width = carTextureSizes[spriteIdx];
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_KILL;
        }

        if (type == ENTITY_TYPE_WALL)
        {
            e.flags = ENTITY_FLAG_KILL;
            e.color = DARKGREEN;
        }

        if (type == ENTITY_TYPE_WIN)
        {
            e.sprite = game.textures.win;
            e.position.x = e.rec.x + GRID_UNIT/2; //TEMP until sprites
            e.position.y = e.rec.y + GRID_UNIT/2;
            game.winCount++;
        }
        arrpush(game.entities, e);
    }
}

void FreeGameState(void)
{
    FreeRaylibAssets(&game.assets);
    arrfree(game.entities);
}

// Update & Draw
// ----------------------------------------------------------------------------

void UpdateGameFrame(void)
{
    // Debug:
    if (IsKeyPressed(KEY_K))
        KillFrog();

    // Pause
    if (input.player.pause || (game.isPaused && input.menu.cancel))
    {
        static float previousTextFade = 0.0f;
        if (!game.isPaused)
        {
            game.isPaused = true;
            ChangeUiMenu(UI_MENU_PAUSE);
            previousTextFade = ui.textFade;
            ui.textFade = 1.0f;
        }
        else
        {
            game.isPaused = false;
            ui.currentMenu = UI_MENU_NONE;
            ui.textFade = previousTextFade;
        }
        PlaySound(ui.sounds.menu);
    }

    if (!game.isPaused)
    {
        // Current level win condition
        if (game.winCount == 0)
        {
            FreeGameState();
            InitGameState();
            game.currentScreen = SCREEN_GAMEPLAY;
            SetTimedMessage("Winner!", 100, 3.0f);
        }

        // Update entities
        for (int i = 0; i < arrlen(game.entities); i++)
        {
            Entity *e = &game.entities[i];

            if (e->type == ENTITY_TYPE_FROG)
                UpdateFrog(e);
            if (e->type == ENTITY_TYPE_WIN)
                UpdateWinZone(e);

            if (e->flags & ENTITY_FLAG_KILL)
            {
                UpdateHostile(e);
            }
            if (e->flags & ENTITY_FLAG_PLATFORM)
            {
                UpdatePlatform(e);
            }
            if (e->flags & ENTITY_FLAG_MOVE)
            {
                MoveEntity(e);
            }
        }

        if (game.turtleTimer > 0)
        {
            game.turtleTimer -= game.frameTime;
        }
        else
        {
            game.turtleTimer = 0.3333f;
            game.turtleTextureOffset = fmodf(game.turtleTextureOffset + game.textures.turtle.width, 48.0f);
        }
    }
    // Prevent input after resuming pause
    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && game.isInputDisabledFromResume)
        game.isInputDisabledFromResume = false;

    // Update user interface elements and logic
    UpdateUiFrame();
}

void UpdateFrog(Entity *frog)
{
    // track frog to moving platform
    if (frog->isOnPlatform)
    {
        frog->position.x += frog->platformMove*game.frameTime;
        frog->seekPos.x += frog->platformMove*game.frameTime;
        frog->bufferPos.x += frog->platformMove*game.frameTime;
    }

    // wrap around screen edge
    bool pastLeftEdge = (frog->position.x + frog->radius < game.gridStart.x);
    if (pastLeftEdge)
    {
        frog->position.x += GRID_WIDTH;
        frog->seekPos.x += GRID_WIDTH;
        frog->bufferPos.x += GRID_WIDTH;
        KillFrog();
    }
    bool pastRightEdge = (frog->position.x - frog->radius > game.gridStart.x + GRID_WIDTH);
    if (pastRightEdge)
    {
        frog->position.x -= GRID_WIDTH;
        frog->seekPos.x -= GRID_WIDTH;
        frog->bufferPos.x -= GRID_WIDTH;
        KillFrog();
    }

    bool onLeftEdge = (frog->position.x - frog->radius < game.gridStart.x);
    bool onRightEdge = (frog->position.x + frog->radius > game.gridStart.x + GRID_WIDTH);
    frog->isWrapping = onLeftEdge || onRightEdge;

    // respawn frog
    if (frog->isDead)
    {
        game.deathTimer -= game.frameTime;

        if (game.deathTimer < 0)
        {
            frog->position = game.spawnPos;
            frog->seekPos = game.spawnPos;
            frog->bufferPos= game.spawnPos;
            frog->isDead = false;
            frog->isDrowned = false;
        }

        return; // no update
    }

    // frog drowned in river (lethal rapids, I guess?)
    if (!frog->isOnPlatform &&
        CheckCollisionPointRec(frog->position, game.background.water))
    {
        KillFrog();
        frog->isDrowned = true;
        return;
    }
    frog->isOnPlatform = false;

    // check if frog reached destination
    if (frog->isMoving && Vector2Equals(frog->position, frog->seekPos))
    {
        // move to possible buffered position
        if (frog->isMoveBuffered)
        {
            frog->seekPos = frog->bufferPos;
            frog->isMoveBuffered = false;
        }
        else frog->isMoving = false;
    }

    // set movement vector
    bool moveInput = (input.player.moveUp   || input.player.moveDown ||
                      input.player.moveLeft || input.player.moveRight);

    if (moveInput)
    {
        Vector2 moveVector = Vector2Zero();
        if      (input.player.moveUp)    moveVector.y -= GRID_UNIT;
        else if (input.player.moveDown)  moveVector.y += GRID_UNIT;
        else if (input.player.moveLeft)  moveVector.x -= GRID_UNIT;
        else if (input.player.moveRight) moveVector.x += GRID_UNIT;

        Vector2 newSeekPos = Vector2Add(frog->position, moveVector);

        // no moving past screen edge
        pastLeftEdge        = (newSeekPos.x + frog->radius < game.gridStart.x + GRID_UNIT);
        pastRightEdge       = (newSeekPos.x - frog->radius > game.gridStart.x + GRID_WIDTH - GRID_UNIT);
        bool pastBottomEdge = (newSeekPos.y - frog->radius > game.gridStart.y + GRID_HEIGHT);
        if (pastLeftEdge || pastRightEdge || pastBottomEdge) return;

        Vector2 newBufferPos = Vector2Add(frog->seekPos, moveVector);

        // set frog seek position
        if (!frog->isMoving)
        {
            frog->isMoving = true;
            frog->seekPos = newSeekPos;
            // frog->bufferPos = newSeekPos;
        }
        // set buffered position
        else if (!frog->isMoveBuffered && !Vector2Equals(frog->bufferPos, newBufferPos))
        {
            pastLeftEdge        = (newBufferPos.x + frog->radius < game.gridStart.x + GRID_UNIT);
            pastRightEdge       = (newBufferPos.x - frog->radius > game.gridStart.x + GRID_WIDTH - GRID_UNIT);
            pastBottomEdge = (newBufferPos.y - frog->radius > game.gridStart.y + GRID_HEIGHT);
            if (pastLeftEdge || pastRightEdge || pastBottomEdge) return;

            frog->bufferPos = newBufferPos;
            frog->isMoveBuffered = true;
        }
    }

    // move frog towards next position
    if (frog->isMoving)
    {
        Vector2 newPos = Vector2MoveTowards(frog->position, frog->seekPos, frog->speed*game.frameTime);
        Vector2 moveDelta = Vector2Subtract(frog->position, newPos);
        frog->position = newPos;

        // set sprite
        frog->spriteOffset.x = 0;

        if (moveDelta.x > 0) frog->angle = 270;
        if (moveDelta.x < 0) frog->angle = 90;
        if (moveDelta.y > 0) frog->angle = 0;
        if (moveDelta.y < 0) frog->angle = 180;
    }
    else frog->spriteOffset.x = 32;
}

void UpdateHostile(Entity *hostile)
{
    if (!game.frog->isDead &&
        CheckCollisionCircleRec(game.frog->position, game.frog->radius*0.75f, hostile->rec))
    {
        KillFrog();
    }
}

void UpdatePlatform(Entity *platform)
{
    bool colliding = false;
    if (platform->isWrapping)
    {
        Rectangle platformWrapLeft = platform->rec;
        platformWrapLeft.x += GRID_WIDTH;
        Rectangle platformWrapRight = platform->rec;
        platformWrapRight.x -= GRID_WIDTH;
        colliding = (CheckCollisionPointRec(game.frog->position, platformWrapLeft) ||
                     CheckCollisionPointRec(game.frog->position, platformWrapRight));
    }

    if (!game.frog->isOnPlatform && !game.frog->isDrowned &&
        (colliding |= CheckCollisionPointRec(game.frog->position, platform->rec)))
    {
        game.frog->isOnPlatform = true;
    }

    if (colliding && game.frog->isOnPlatform)
    {
        game.frog->platformMove = platform->speed;
    }

    platform->position.x = platform->rec.x + platform->radius; //TEMP until sprites
    platform->position.y = platform->rec.y + platform->radius;
}

void UpdateWinZone(Entity *box)
{
    if (!box->isWin && CheckCollisionPointRec(game.frog->position, box->rec))
    {
        box->isWin = true;
        box->flags |= ENTITY_FLAG_KILL;
        game.winCount--;
        game.frog->isWin = false;
        game.frog->position = game.spawnPos;
        game.frog->seekPos = game.spawnPos;
        game.frog->bufferPos= game.spawnPos;
    }
}

void MoveEntity(Entity *e)
{
    // wrap rectangle entities
    if (e->rec.width > 0)
    {
        bool pastLeftEdge = (e->rec.x + e->rec.width < game.gridStart.x);
        if (pastLeftEdge) e->rec.x += GRID_WIDTH;
        bool pastRightEdge = (e->rec.x > game.gridStart.x + GRID_WIDTH);
        if (pastRightEdge) e->rec.x -= GRID_WIDTH;

        bool onLeftEdge = (e->rec.x < game.gridStart.x);
        bool onRightEdge = (e->rec.x + e->rec.width > game.gridStart.x + GRID_WIDTH);
        e->isWrapping = onLeftEdge || onRightEdge;
    }

    e->rec.x += e->speed*game.frameTime;
}

void DrawGameFrame(void)
{
    ClearBackground(DARKGRAY);

    // Draw background elements
    // ----------------------------------------------------------------------------
    DrawRectangleRec(game.background.water, ColorBrightness(DARKBLUE, -0.5f));
    DrawRectangleRec(game.background.grassTop, DARKPURPLE);
    DrawRectangleRec(game.background.grassBottom, DARKPURPLE);

    // Draw entities
    // ----------------------------------------------------------------------------
    for (int i = 0; i < arrlen(game.entities); i++)
    {
        Entity *e = &game.entities[i];

        // Platforms
        if (e->type == ENTITY_TYPE_WALL)
        {
            DrawRectangleRec(e->rec, e->color);
            if (e->isWrapping)
            {
                DrawRectangleV((Vector2){ e->rec.x + GRID_WIDTH, e->rec.y },
                               (Vector2){ e->rec.width, e->rec.height }, e->color);
                DrawRectangleV((Vector2){ e->rec.x - GRID_WIDTH, e->rec.y },
                               (Vector2){ e->rec.width, e->rec.height }, e->color);
            }
        }

        // Win zones
        if (e->type == ENTITY_TYPE_WIN && e->isWin)
        {
            // DrawRectangleRec(e->rec, e->color);
            // DrawCircleV(e->position, game.frog->radius, game.frog->color);
            DrawSpriteOnRectangle(&game.textures.atlas, e->sprite, e->rec, e->angle);
        }

        if (e->type == ENTITY_TYPE_CAR || e->type == ENTITY_TYPE_TURTLE)
        {
            Rectangle sprite = e->sprite;
            if (e->type == ENTITY_TYPE_TURTLE)
            {
                sprite.x += game.turtleTextureOffset;
            }
            DrawSpriteOnRectangle(&game.textures.atlas, sprite, e->rec, e->angle);
            if (e->isWrapping)
            {
                Rectangle wrapLeftRec = e->rec;
                wrapLeftRec.x += GRID_WIDTH;
                Rectangle wrapRightRec = e->rec;
                wrapRightRec.x -= GRID_WIDTH;
                DrawSpriteOnRectangle(&game.textures.atlas, sprite, wrapLeftRec, e->angle);
                DrawSpriteOnRectangle(&game.textures.atlas, sprite, wrapRightRec, e->angle);
            }
        }

        if (e->type == ENTITY_TYPE_LOG)
        {
            Rectangle wrapLeftRec = e->rec;
            Rectangle wrapRightRec = e->rec;
            wrapLeftRec.width = GRID_UNIT;
            wrapLeftRec.x += GRID_WIDTH;
            wrapRightRec.width = GRID_UNIT;
            wrapRightRec.x -= GRID_WIDTH;

            Rectangle sprite = e->sprite;
            Rectangle rec = e->rec;
            rec.width = GRID_UNIT;
            float logWidth = e->rec.width/GRID_UNIT;
            for (int i = 1; i <= logWidth; i++)
            {
                if (i > 1)
                    sprite.x = e->sprite.x + 16; // log middle
                if (i == logWidth)
                    sprite.x = e->sprite.x + 32; // log end
                DrawSpriteOnRectangle(&game.textures.atlas, sprite, rec, e->angle);

                if (e->isWrapping)
                {
                    DrawSpriteOnRectangle(&game.textures.atlas, sprite, wrapLeftRec, e->angle);
                    DrawSpriteOnRectangle(&game.textures.atlas, sprite, wrapRightRec, e->angle);
                    wrapLeftRec.x += GRID_UNIT;
                    wrapRightRec.x += GRID_UNIT;
                }
                rec.x += GRID_UNIT;
            }
        }

        // Frog
        if (e->type == ENTITY_TYPE_FROG)
        {
            Rectangle sprite;
            float angle;
            if (e->isDead)
            {
                sprite = game.textures.dead;
                angle = 0;
            }
            else
            {
                angle = e->angle;
                sprite = e->sprite;
                sprite.x += e->spriteOffset.x;
                sprite.y += e->spriteOffset.y;
            }

            DrawSpriteOnCircle(&game.textures.atlas, sprite, e->position, e->radius, angle, 1.0f);
            if (e->isWrapping)
            {
                Vector2 wrapLeftPos = { e->position.x + GRID_WIDTH, e->position.y };
                Vector2 wrapRightPos = { e->position.x - GRID_WIDTH, e->position.y };
                DrawSpriteOnCircle(&game.textures.atlas, sprite, wrapLeftPos, e->radius, angle, 1.0f);
                DrawSpriteOnCircle(&game.textures.atlas, sprite, wrapRightPos, e->radius, angle, 1.0f);
            }
        }
    }

    // Draw game border (outside of grid)
    // ----------------------------------------------------------------------------
    // left, right, top, bottom
    DrawRectangleV((Vector2){ 0, 0 },
                   (Vector2){ VIRTUAL_WIDTH - GRID_WIDTH + GRID_UNIT/2, VIRTUAL_HEIGHT },
                   BLACK);
    DrawRectangleV((Vector2){ game.gridStart.x + GRID_WIDTH - GRID_UNIT, 0 },
                   (Vector2){ VIRTUAL_WIDTH - GRID_WIDTH + GRID_UNIT, VIRTUAL_HEIGHT },
                   BLACK);
    DrawRectangleV((Vector2){ game.gridStart.x, 0 },
                   (Vector2){ GRID_WIDTH, (VIRTUAL_HEIGHT + GRID_UNIT - (game.gridStart.y + GRID_HEIGHT)) },
                   BLACK);
    DrawRectangleV((Vector2){ game.gridStart.x, (game.gridStart.y + GRID_HEIGHT - GRID_UNIT) },
                   (Vector2){ GRID_WIDTH, (VIRTUAL_HEIGHT + GRID_UNIT - (game.gridStart.y + GRID_HEIGHT)) },
                   BLACK);
}

Vector2 GetGridPosition(int col, int row)
{
    int index = row*GRID_RES_X + col;
    return (Vector2){ game.grid[index].x, game.grid[index].y };
}

void KillFrog(void)
{
    game.frog->isDead = true;
    game.deathTimer = 0.5f;
}
