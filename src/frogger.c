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
        .camera.offset = (Vector2){ viewport.renderTexWidth/2, viewport.renderTexHeight/2 },
        .camera.zoom = viewport.renderTexHeight/VIRTUAL_HEIGHT,

        .lives = 3,

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
    game.textures.car         = (Rectangle){ 48,   0, 16, 16 };
    game.textures.frog        = (Rectangle){  0,   0, 16, 16 };
    game.textures.grassPurple = (Rectangle){ 48,  32, 16, 16 };
    game.textures.grassGreen  = (Rectangle){ 64,  24, 16, 24 };
    game.textures.deadFrog    = (Rectangle){ 48,  48, 16, 16 };
    game.textures.turtle      = (Rectangle){  0,  80, 16, 16 };
    game.textures.winFrog     = (Rectangle){ 48,  96, 16, 16 };
    game.textures.log         = (Rectangle){ 96, 128, 16, 16 };
    game.textures.life        = (Rectangle){ 48,  16,  8,  8 };
    game.textures.level       = (Rectangle){ 96, 128, 16, 16 };

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
    Vector2 frogSpawnPos = GetGridPosition(8, 14);
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
    game.background.grassBottom.height = GRID_UNIT;
}

void CreateRow(EntityType type, int row, char *pattern, float speed)
{

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
    float entityWidth = GRID_UNIT;
    bool isLeftWall = false;
    bool isExtending = false;

    for (char *c = pattern; *c != '\0'; c++)
    {
        if (*c == '.') currentPos.x += GRID_UNIT/2;
        if (*c == '_') currentPos.x += GRID_UNIT;

        if (*c == '_' || *c == '.') isExtending = false;
        if (isExtending)
        {
            arrlast(game.entities).rec.width += entityWidth;
            currentPos.x += entityWidth;
        }

        if (*c == '_' || *c == '.' || isExtending)
            continue;

        Entity e = { 0 };
        e.type = type;
        e.speed = speed;
        e.rec.x = currentPos.x;
        e.rec.y = currentPos.y;
        e.rec.width = entityWidth;
        currentPos.x += entityWidth;
        e.rec.height = GRID_UNIT;

        isExtending = true;
        if (type == ENTITY_TYPE_TURTLE)
        {
            e.sprite = game.textures.turtle;
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_PLATFORM;
            isExtending = false;
        }

        if (type == ENTITY_TYPE_LOG)
        {
            e.sprite = game.textures.log;
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_PLATFORM;
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
            e.sprite = game.textures.grassGreen;
            if (isLeftWall)
                e.spriteOffset.x = 32;
            isLeftWall = !isLeftWall;
            e.flags = ENTITY_FLAG_KILL;
            e.rec.height += GRID_UNIT/2;
            e.rec.y -= GRID_UNIT/2;
            e.color = DARKGREEN;
            isExtending = false;
        }

        if (type == ENTITY_TYPE_WIN)
        {
            e.sprite = game.textures.winFrog;
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

        // Game over condition
        if ((game.lives == 0) && !game.gameOver)
        {
            game.gameOver = true;
            SetTimedMessage("Game Over!", 100, 3.0f);
        }
        if (game.gameOver && (ui.messageTimer < EPSILON))
        {
            FreeGameState();
            InitGameState();
            game.currentScreen = SCREEN_GAMEPLAY;
        }

        // Update entities
        for (int i = 0; i < arrlen(game.entities); i++)
        {
            Entity *e = &game.entities[i];

            if (e->type == ENTITY_TYPE_FROG)
                UpdateFrog();
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

        if (game.animateTimer > 0)
        {
            game.animateTimer -= game.frameTime;
        }
        else
        {
            game.animateTimer = 0.3333f;
            game.animateTextureOffset = fmodf(game.animateTextureOffset + game.textures.turtle.width, 48.0f);
        }
    }
    // Prevent input after resuming pause
    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && game.isInputDisabledFromResume)
        game.isInputDisabledFromResume = false;

    // Update user interface elements and logic
    UpdateUiFrame();
}

void UpdateFrog()
{
    if (game.lives == 0) return;

    // track game.frog to moving platform
    if (game.frog->isOnPlatform)
    {
        game.frog->position.x += game.frog->platformMove*game.frameTime;
        game.frog->seekPos.x += game.frog->platformMove*game.frameTime;
        game.frog->bufferPos.x += game.frog->platformMove*game.frameTime;
    }

    // wrap around screen edge
    bool pastLeftEdge = (game.frog->position.x + game.frog->radius < game.gridStart.x);
    if (pastLeftEdge)
    {
        game.frog->position.x += GRID_WIDTH;
        game.frog->seekPos.x += GRID_WIDTH;
        game.frog->bufferPos.x += GRID_WIDTH;
        KillFrog();
    }
    bool pastRightEdge = (game.frog->position.x - game.frog->radius > game.gridStart.x + GRID_WIDTH);
    if (pastRightEdge)
    {
        game.frog->position.x -= GRID_WIDTH;
        game.frog->seekPos.x -= GRID_WIDTH;
        game.frog->bufferPos.x -= GRID_WIDTH;
        KillFrog();
    }

    bool onLeftEdge = (game.frog->position.x - game.frog->radius < game.gridStart.x);
    bool onRightEdge = (game.frog->position.x + game.frog->radius > game.gridStart.x + GRID_WIDTH);
    game.frog->isWrapping = onLeftEdge || onRightEdge;

    // respawn game.frog
    if (game.frog->isDead)
    {
        game.deathTimer -= game.frameTime;

        if (game.deathTimer < 0)
        {
            game.frog->position = game.spawnPos;
            game.frog->seekPos = game.spawnPos;
            game.frog->bufferPos= game.spawnPos;
            game.frog->isDead = false;
            game.frog->isDrowned = false;
        }

        return; // no update
    }

    // game.frog drowned in river (lethal rapids, I guess?)
    if (!game.frog->isOnPlatform &&
        CheckCollisionPointRec(game.frog->position, game.background.water))
    {
        KillFrog();
        game.frog->isDrowned = true;
        return;
    }
    game.frog->isOnPlatform = false;

    // check if game.frog reached destination
    if (game.frog->isMoving && Vector2Equals(game.frog->position, game.frog->seekPos))
    {
        // move to possible buffered position
        if (game.frog->isMoveBuffered)
        {
            game.frog->seekPos = game.frog->bufferPos;
            game.frog->isMoveBuffered = false;
        }
        else game.frog->isMoving = false;
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

        Vector2 newSeekPos = Vector2Add(game.frog->position, moveVector);

        // no moving past screen edge
        pastLeftEdge        = (newSeekPos.x + game.frog->radius < game.gridStart.x + GRID_UNIT);
        pastRightEdge       = (newSeekPos.x - game.frog->radius > game.gridStart.x + GRID_WIDTH - GRID_UNIT);
        bool pastBottomEdge = (newSeekPos.y - game.frog->radius > game.gridStart.y + GRID_HEIGHT);
        if (pastLeftEdge || pastRightEdge || pastBottomEdge) return;

        Vector2 newBufferPos = Vector2Add(game.frog->seekPos, moveVector);

        // set game.frog seek position
        if (!game.frog->isMoving)
        {
            game.frog->isMoving = true;
            game.frog->seekPos = newSeekPos;
            // game.frog->bufferPos = newSeekPos;
        }
        // set buffered position
        else if (!game.frog->isMoveBuffered && !Vector2Equals(game.frog->bufferPos, newBufferPos))
        {
            pastLeftEdge        = (newBufferPos.x + game.frog->radius < game.gridStart.x + GRID_UNIT);
            pastRightEdge       = (newBufferPos.x - game.frog->radius > game.gridStart.x + GRID_WIDTH - GRID_UNIT);
            pastBottomEdge = (newBufferPos.y - game.frog->radius > game.gridStart.y + GRID_HEIGHT);
            if (pastLeftEdge || pastRightEdge || pastBottomEdge) return;

            game.frog->bufferPos = newBufferPos;
            game.frog->isMoveBuffered = true;
        }
    }

    // move game.frog towards next position
    if (game.frog->isMoving)
    {
        Vector2 newPos = Vector2MoveTowards(game.frog->position, game.frog->seekPos, game.frog->speed*game.frameTime);
        Vector2 moveDelta = Vector2Subtract(game.frog->position, newPos);
        game.frog->position = newPos;

        // set sprite
        float distFromDest = Vector2Length(Vector2Subtract(game.frog->position, game.frog->seekPos));
        if (distFromDest < GRID_UNIT*0.2f)
            game.frog->spriteOffset.x = 32; // not hopping
        else
            game.frog->spriteOffset.x = 0; // hopping

        if (moveDelta.x > 0) game.frog->angle = 270;
        if (moveDelta.x < 0) game.frog->angle = 90;
        if (moveDelta.y > 0) game.frog->angle = 0;
        if (moveDelta.y < 0) game.frog->angle = 180;
    }
    else game.frog->spriteOffset.x = 32;
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
}

void UpdateWinZone(Entity *zone)
{
    if (!zone->isWin && CheckCollisionPointRec(game.frog->position, zone->rec))
    {
        zone->isWin = true;
        zone->flags |= ENTITY_FLAG_KILL;
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
    ClearBackground(ColorBrightness(DARKGRAY, -0.8f));

    // Draw background elements
    // ----------------------------------------------------------------------------
    DrawRectangleRec(game.background.water, (Color){ 0x00, 0x04, 0x4a, 255 });
    DrawGrass(game.background.grassTop);
    DrawGrass(game.background.grassBottom);


    // Draw entities
    // ----------------------------------------------------------------------------
    for (int i = 0; i < arrlen(game.entities); i++)
    {
        Entity *e = &game.entities[i];

        // Draw grass on top of screen
        if (e->type == ENTITY_TYPE_WALL)
        {
            Rectangle sprite = e->sprite;
            sprite.x += e->spriteOffset.x;
            sprite.y += e->spriteOffset.y;
            DrawSpriteOnRectangle(&game.textures.atlas, sprite, e->rec, e->angle);
        }

        // Win zones
        if (e->type == ENTITY_TYPE_WIN)
        {
            // DrawRectangleRec(e->rec, e->color);
            // DrawCircleV(e->position, game.frog->radius, game.frog->color);
            Rectangle topGrass = game.textures.grassGreen;
            topGrass.x += 16;
            topGrass.height -= 8;
            Rectangle grassRec = e->rec;
            grassRec.y -= GRID_UNIT/2;
            DrawSpriteOnRectangle(&game.textures.atlas, topGrass, grassRec, e->angle);
            if (e->isWin)
                DrawSpriteOnRectangle(&game.textures.atlas, e->sprite, e->rec, e->angle);
        }

        if (e->type == ENTITY_TYPE_CAR || e->type == ENTITY_TYPE_TURTLE)
        {
            Rectangle sprite = e->sprite;
            if (e->type == ENTITY_TYPE_TURTLE)
            {
                sprite.x += game.animateTextureOffset;
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
                sprite = game.textures.deadFrog;
                angle = 0;
            }
            else
            {
                angle = e->angle;
                sprite = e->sprite;
                sprite.x += e->spriteOffset.x;
                sprite.y += e->spriteOffset.y;
            }

            DrawSpriteOnCircle(&game.textures.atlas, sprite, e->position, GRID_UNIT/2, angle);
            if (e->isWrapping)
            {
                Vector2 wrapLeftPos = { e->position.x + GRID_WIDTH, e->position.y };
                Vector2 wrapRightPos = { e->position.x - GRID_WIDTH, e->position.y };
                DrawSpriteOnCircle(&game.textures.atlas, sprite, wrapLeftPos, GRID_UNIT/2, angle);
                DrawSpriteOnCircle(&game.textures.atlas, sprite, wrapRightPos, GRID_UNIT/2, angle);
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

    // Draw HUD
    // ----------------------------------------------------------------------------
    Vector2 lifePos = game.gridStart;
    lifePos.x += GRID_UNIT;
    lifePos.y += GRID_HEIGHT - GRID_UNIT;
    for (int i = 0; i < game.lives; i++)
    {
        Rectangle lifeRec = { lifePos.x, lifePos.y, GRID_UNIT/2, GRID_UNIT/2 };
        DrawTexturePro(game.textures.atlas, game.textures.life, lifeRec, Vector2Zero(), 0, WHITE);
        lifePos.x += GRID_UNIT/2;
    }
}

void DrawGrass(Rectangle grassRec)
{
    int tileAmount = (int)(grassRec.width/GRID_UNIT);
    grassRec.width = GRID_UNIT + 0.1f;

    for (int i = 0; i < tileAmount; i++)
    {
        DrawTexturePro(game.textures.atlas, game.textures.grassPurple, grassRec, Vector2Zero(), 0, WHITE);
        grassRec.x += GRID_UNIT;
    }
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
    game.lives--;
}
