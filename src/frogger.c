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

    // Create entities
    // ----------------------------------------------------------------------------

    // Logs and Lilies
    int spawnRow = 0;
    CreateRow(ENTITY_TYPE_WALL,    spawnRow,   "Oo_OO_OO_OO_OO_oO",  0);
    CreateRow(ENTITY_TYPE_WIN,     spawnRow++, "_.O__O__O__O__O._",  0);
    CreateRow(ENTITY_TYPE_LOG,     spawnRow++, "_OOOo__OOOo__OOOo",  GRID_UNIT*1.1f);
    CreateRow(ENTITY_TYPE_LILYPAD, spawnRow++, "___OO_.OO_.OO_.OO",  -GRID_UNIT*1.5f);
    CreateRow(ENTITY_TYPE_LOG,     spawnRow++, "__OOOOo__.OOOOo",    GRID_UNIT*2.5f);
    CreateRow(ENTITY_TYPE_LOG,     spawnRow++, "___OOo__.OOo__.OOo", GRID_UNIT*0.5f);
    CreateRow(ENTITY_TYPE_LILYPAD, spawnRow++, "_OOO_OOO_OOO_OOO",   -GRID_UNIT*1.5f);

    // Frog
    Entity frog = {
        .type = ENTITY_TYPE_FROG,
        .speed = BASE_SPEED*5.0f,
        .radius = GRID_UNIT*0.4f,
        .color = GREEN,
    };
    Vector2 frogSpawnPos = GetGridPosition(GRID_RES_X/2, 12);
    frog.position = frogSpawnPos;
    frog.position.x += GRID_UNIT/2;
    frog.position.y += GRID_UNIT/2;
    frog.seekPos = frogSpawnPos;
    frog.bufferPos = frogSpawnPos;
    arrpush(game.entities, frog);
    game.frog = &arrlast(game.entities);
    game.spawnPos = frog.position;

    // Cars
    spawnRow = 7;
    CreateRow(ENTITY_TYPE_CAR, spawnRow++, "________.OO___.OO", -BASE_SPEED);
    CreateRow(ENTITY_TYPE_CAR, spawnRow++, "O_______________",  BASE_SPEED*0.6f);
    CreateRow(ENTITY_TYPE_CAR, spawnRow++, "_______O___O___O",  -BASE_SPEED*0.6f);
    CreateRow(ENTITY_TYPE_CAR, spawnRow++, "_______O___O___O",  BASE_SPEED*0.4f);
    CreateRow(ENTITY_TYPE_CAR, spawnRow++, "______O___.O___.O", -BASE_SPEED*0.4f);

    // Background rectangles (river, grass, road)
    game.background.water.x = game.gridStart.x;
    game.background.water.y = game.gridStart.y + GRID_UNIT;
    game.background.water.width = GRID_WIDTH;
    game.background.water.height = GRID_UNIT*5;

    game.background.grassTop.x = game.gridStart.x;
    game.background.grassTop.y = GetGridPosition(0, 6).y;
    game.background.grassTop.width = GRID_WIDTH;
    game.background.grassTop.height = GRID_UNIT;

    game.background.grassBottom.x = game.gridStart.x;
    game.background.grassBottom.y = GetGridPosition(0, 12).y;
    game.background.grassBottom.width = GRID_WIDTH;
    game.background.grassBottom.height = GRID_UNIT*2;
}

void CreateRow(EntityType type, int row, char *pattern, float speed)
{
    Color logColor = ColorBrightness(BROWN, 0.0f - (float)row*0.10f);
    Color carColors[] = { RED, YELLOW, PURPLE, ORANGE, PINK };
    int carColorIdx = row - 7;
    Vector2 currentPos = GetGridPosition(0, row);
    float widthExtend = 0;
    bool isExtending = false;

    for (char *c = pattern; *c != '\0'; c++)
    {
        if (*c == 'o') widthExtend = GRID_UNIT/2;
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
        if (type == ENTITY_TYPE_LILYPAD)
        {
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_PLATFORM;
            e.radius = GRID_UNIT/2; //TEMP until sprites
            e.position.x = e.rec.x + e.radius; //TEMP until sprites
            e.position.y = e.rec.y + e.radius;
            e.color = DARKGREEN;
            isExtending = false;
        }

        if (type == ENTITY_TYPE_LOG)
        {
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_PLATFORM;
            e.color = logColor;
        }

        if (type == ENTITY_TYPE_CAR)
        {
            e.flags = ENTITY_FLAG_MOVE | ENTITY_FLAG_KILL;
            e.color = carColors[carColorIdx];
        }

        if (type == ENTITY_TYPE_WALL)
        {
            e.flags = ENTITY_FLAG_KILL;
            e.color = DARKGREEN;
        }

        if (type == ENTITY_TYPE_WIN)
        {
            e.position.x = e.rec.x + GRID_UNIT/2; //TEMP until sprites
            e.position.y = e.rec.y + GRID_UNIT/2;
            e.color = DARKBLUE;
            game.winCount++;
        }
        arrpush(game.entities, e);
    }
}

void FreeGameState(void)
{
    // FreeRaylibAssets(&game.assets);
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

        if (game.deathTimer < EPSILON)
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
        pastLeftEdge = (newSeekPos.x + frog->radius < game.gridStart.x + GRID_UNIT);
        pastRightEdge = (newSeekPos.x - frog->radius > game.gridStart.x + GRID_WIDTH - GRID_UNIT);
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
            frog->bufferPos = newBufferPos;
            frog->isMoveBuffered = true;
        }
    }

    // move frog towards next position
    if (frog->isMoving)
    {
        Vector2 newPos = Vector2MoveTowards(frog->position, frog->seekPos, frog->speed*game.frameTime);
        frog->position = newPos;
    }
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
    DrawRectangleRec(game.background.water, DARKBLUE);
    DrawRectangleRec(game.background.grassTop, DARKPURPLE);
    DrawRectangleRec(game.background.grassBottom, DARKPURPLE);

    // Draw entities
    // ----------------------------------------------------------------------------
    for (int i = 0; i < arrlen(game.entities); i++)
    {
        Entity *e = &game.entities[i];

        if (e->type != ENTITY_TYPE_FROG &&
            e->type != ENTITY_TYPE_LILYPAD &&
            e->type != ENTITY_TYPE_WIN)
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

        if (e->type == ENTITY_TYPE_WIN)
        {
            DrawRectangleRec(e->rec, e->color);
            if (e->isWin)
                DrawCircleV(e->position, game.frog->radius, game.frog->color);
        }

        if (e->type == ENTITY_TYPE_FROG)
        {
            Color frogColor = (e->isDead)? MAROON : e->color;
            DrawCircleV(e->position, e->radius, frogColor);
            if (e->isWrapping)
            {
                DrawCircle((int)(e->position.x + GRID_WIDTH),
                           (int)e->position.y, e->radius, frogColor);
                DrawCircle((int)(e->position.x - GRID_WIDTH),
                           (int)e->position.y, e->radius, frogColor);
            }
        }

        if (e->type == ENTITY_TYPE_LILYPAD)
        {
            DrawCircleV(e->position, e->radius, e->color);
            if (e->isWrapping)
            {
                DrawCircleV((Vector2){ e->position.x + GRID_WIDTH, e->position.y },
                            e->radius, e->color);
                DrawCircleV((Vector2){ e->position.x - GRID_WIDTH, e->position.y },
                            e->radius, e->color);
            }
        }
    }

    // Draw game border (outside of grid)
    // ----------------------------------------------------------------------------
    // left, right, top, bottom
    DrawRectangle(-1, 0,
                  (int)(game.gridStart.x + GRID_UNIT + 1),
                  (int)VIRTUAL_HEIGHT, BLACK);
    DrawRectangle((int)(game.gridStart.x + GRID_WIDTH - GRID_UNIT), 0,
                  (int)(VIRTUAL_WIDTH - (game.gridStart.x + GRID_WIDTH) + GRID_UNIT),
                  (int)VIRTUAL_HEIGHT, BLACK);
    DrawRectangle((int)game.gridStart.x, 0,
                  (int)VIRTUAL_HEIGHT,
                  (int)(VIRTUAL_HEIGHT - (game.gridStart.y + GRID_HEIGHT)),
                  BLACK);
    DrawRectangle((int)game.gridStart.x, (int)(game.gridStart.y + GRID_HEIGHT),
                  (int)VIRTUAL_HEIGHT,
                  (int)(VIRTUAL_HEIGHT - (game.gridStart.y + GRID_HEIGHT)),
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
