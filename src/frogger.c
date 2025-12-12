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

    // Create entities
    // ----------------------------------------------------------------------------

    // Logs and Lilies
    int spawnRow = 1;
    CreateLogRow(spawnRow++,     "OOOO.OOOO.OOOO..",  GRID_UNIT, 4);
    CreateLilypadRow(spawnRow++, "....OO..OO..OO..", -GRID_UNIT*1.5f);
    CreateLogRow(spawnRow++,     "..OOOOO..OOOOO..",  GRID_UNIT*2, 5);
    CreateLogRow(spawnRow++,     "..OO..OO..OO....",  GRID_UNIT*0.5f, 2);
    CreateLilypadRow(spawnRow++, "OOO.OOO.OOO.OOO.", -GRID_UNIT*1.5f);

    // Frog
    Point spawnPoint = { GRID_RES_X/2, 6 };
    Entity frog = {
        .type = ENTITY_TYPE_FROG,
        .speed = BASE_SPEED*5.0f,
        .radius = GRID_UNIT*0.4f,
        .gridIndex = spawnPoint,
        .color = GREEN,
    };
    Vector2 frogPos = GetGridPosition(frog.gridIndex.x, frog.gridIndex.y);
    frog.position = frogPos;
    frog.position.x += GRID_UNIT/2;
    frog.position.y += GRID_UNIT/2;
    frog.seekPos = frogPos;
    frog.bufferPos = frogPos;
    // game.entities.frog = frog;
    arrpush(game.entities, frog);
    game.frog = &arrlast(game.entities);
    game.spawnPos = frog.position;

    // Cars
    spawnRow = 7;
    CreateCarRow(spawnRow++, "....OO...OO.....", -BASE_SPEED,      2);
    CreateCarRow(spawnRow++, ".O..............",  BASE_SPEED*0.6f, 1);
    CreateCarRow(spawnRow++, "...O....O....O..", -BASE_SPEED*0.6f, 1);
    CreateCarRow(spawnRow++, "O.....O...O.....",  BASE_SPEED*0.4f, 1);
    CreateCarRow(spawnRow++, "..O....O....O...", -BASE_SPEED*0.4f, 1);

    // Background rectangles (river, grass, road)
    game.background.water.x = game.gridStart.x;
    game.background.water.y = game.gridStart.y;
    game.background.water.width = GRID_WIDTH;
    game.background.water.height = GRID_UNIT*6;

    game.background.grassTop.x = game.gridStart.x;
    game.background.grassTop.y = GetGridPosition(0, 6).y;
    game.background.grassTop.width = GRID_WIDTH;
    game.background.grassTop.height = GRID_UNIT;

    game.background.grassBottom.x = game.gridStart.x;
    game.background.grassBottom.y = GetGridPosition(0, 12).y;
    game.background.grassBottom.width = GRID_WIDTH;
    game.background.grassBottom.height = GRID_UNIT;
}

void CreateLilypadRow(int row, char *pattern, float speed)
{
    int i = 0;
    char *c = pattern;
    for (;*c != '\0'; c++, i++)
    {
        if (*c == '.') continue;

        Entity lily = {
            .type = ENTITY_TYPE_LILYPAD,
            .speed = speed,
            .radius = GRID_UNIT/2, //TEMP: for drawing circle until sprites
            .rect.width = GRID_UNIT,
            .rect.height = GRID_UNIT*1,
            .gridIndex = { i, row },
            .color = DARKGREEN,
        };
        Vector2 lilyPosition = GetGridPosition(lily.gridIndex.x, lily.gridIndex.y);
        lily.position = lilyPosition; //TEMP
        lily.position.x = lilyPosition.x + GRID_UNIT/2;
        lily.position.y = lilyPosition.y + GRID_UNIT/2;
        lily.rect.x = lilyPosition.x;
        lily.rect.y = lilyPosition.y;
        arrpush(game.entities, lily);
    }
}

void CreateLogRow(int row, char *pattern, float speed, int width)
{
    Color logColor = ColorBrightness(BROWN, 0.0f - (float)row*0.10f);
    int i = 0;
    char *c = pattern;
    for (;*c != '\0'; c++, i++)
    {
        if (*c == '.') continue;

        Entity log = {
            .type = ENTITY_TYPE_LOG,
            .speed = speed,
            .rect.width = GRID_UNIT*width,
            .rect.height = GRID_UNIT*1,
            .gridIndex = { i, row },
            .color = logColor
        };
        Vector2 logPosition = GetGridPosition(log.gridIndex.x, log.gridIndex.y);
        log.rect.x = logPosition.x;
        log.rect.y = logPosition.y;
        arrpush(game.entities, log);

        i += width - 1;
        c += width - 1; // don't make extras when width > 1
    }
}

void CreateCarRow(int row, char *pattern, float speed, int width)
{
    Color carColors[] = { RED, YELLOW, PURPLE, ORANGE, PINK };
    int carColorIdx = row - 7;

    int i = 0;
    char *c = pattern;
    for (;*c != '\0'; c++, i++)
    {
        if (*c == '.') continue;

        Entity car = {
            .type = ENTITY_TYPE_CAR,
            .speed = speed,
            .rect.width = GRID_UNIT*width,
            .rect.height = GRID_UNIT*1,
            .gridIndex = { i, row },
            .color = carColors[carColorIdx],
        };
        Vector2 carPosition = GetGridPosition(car.gridIndex.x, car.gridIndex.y);
        car.rect.x = carPosition.x;
        car.rect.y = carPosition.y;
        arrpush(game.entities, car);

        i += width - 1;
        c += width - 1; // don't make extras when width > 1
    }

    carColorIdx++;
}

// void FreeGameState(void) // TODO: free assets properly
// {
//     FreeRaylibAssets(&game.assets);
// }

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
        // Update entities
        for (int i = 0; i < arrlen(game.entities); i++)
        {
            Entity *e = &game.entities[i];

            if (e->type == ENTITY_TYPE_CAR)
            {
                CollideCarFrog(e);
            }
            if (e->type == ENTITY_TYPE_LILYPAD || e->type == ENTITY_TYPE_LOG)
            {
                UpdatePlatform(e);
                e->position.x = e->rect.x + e->radius; //TEMP
                e->position.y = e->rect.y + e->radius;
            }
            if (e->type != ENTITY_TYPE_FROG)
            {
                MoveEntity(e);
            }
            if (e->type == ENTITY_TYPE_FROG)
                UpdateFrog(e);
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
    if (frog->onPlatform)
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
    frog->wrapping = onLeftEdge || onRightEdge;

    // respawn frog
    if (frog->dead)
    {
        game.deathTimer -= game.frameTime;

        if (game.deathTimer < EPSILON)
        {
            frog->position = game.spawnPos;
            frog->seekPos = game.spawnPos;
            frog->bufferPos= game.spawnPos;
            frog->dead = false;
            frog->drowned = false;
        }

        return; // no update
    }

    // frog drowned in river (lethal rapids, I guess?)
    if (!frog->onPlatform &&
        CheckCollisionPointRec(frog->position, game.background.water))
    {
        KillFrog();
        frog->drowned = true;
        return;
    }
    frog->onPlatform = false;

    // check if frog reached destination
    if (frog->moving && Vector2Equals(frog->position, frog->seekPos))
    {
        // move to possible buffered position
        if (frog->moveBuffered)
        {
            frog->seekPos = frog->bufferPos;
            frog->moveBuffered = false;
        }
        else frog->moving = false;
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
        if (pastLeftEdge || pastRightEdge) return;

        Vector2 newBufferPos = Vector2Add(frog->seekPos, moveVector);

        // set frog seek position
        if (!frog->moving)
        {
            frog->moving = true;
            frog->seekPos = newSeekPos;
            // frog->bufferPos = newSeekPos;
        }
        // set buffered position
        else if (!frog->moveBuffered && !Vector2Equals(frog->bufferPos, newBufferPos))
        {
            frog->bufferPos = newBufferPos;
            frog->moveBuffered = true;
        }
    }

    // move frog towards next position
    if (frog->moving)
    {
        Vector2 newPos = Vector2MoveTowards(frog->position, frog->seekPos, frog->speed*game.frameTime);
        frog->position = newPos;
    }
}

void MoveEntity(Entity *e)
{
    // wrap rectangle entities
    if (e->rect.width > 0)
    {
        bool pastLeftEdge = (e->rect.x + e->rect.width < game.gridStart.x);
        if (pastLeftEdge) e->rect.x += GRID_WIDTH;
        bool pastRightEdge = (e->rect.x > game.gridStart.x + GRID_WIDTH);
        if (pastRightEdge) e->rect.x -= GRID_WIDTH;

        bool onLeftEdge = (e->rect.x < game.gridStart.x);
        bool onRightEdge = (e->rect.x + e->rect.width > game.gridStart.x + GRID_WIDTH);
        e->wrapping = onLeftEdge || onRightEdge;
    }

    e->rect.x += e->speed*game.frameTime;
}

void UpdatePlatform(Entity *platform)
{
    bool colliding = false;
    if (platform->wrapping)
    {
        Rectangle platformWrapLeft = platform->rect;
        platformWrapLeft.x += GRID_WIDTH;
        Rectangle platformWrapRight = platform->rect;
        platformWrapRight.x -= GRID_WIDTH;
        colliding = (CheckCollisionPointRec(game.frog->position, platformWrapLeft) ||
                     CheckCollisionPointRec(game.frog->position, platformWrapRight));
    }

    if (!game.frog->onPlatform && !game.frog->drowned &&
        (colliding |= CheckCollisionPointRec(game.frog->position, platform->rect)))
    {
        game.frog->onPlatform = true;
    }

    if (colliding && game.frog->onPlatform)
    {
        game.frog->platformMove = platform->speed;
    }
}

void CollideCarFrog(Entity *car)
{
    if (!game.frog->dead &&
        CheckCollisionCircleRec(game.frog->position, game.frog->radius/2, car->rect))
    {
        KillFrog();
    }
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

        if (e->type != ENTITY_TYPE_FROG && e->type != ENTITY_TYPE_LILYPAD)
        {
            DrawRectangleRec(e->rect, e->color);
            if (e->wrapping)
            {
                DrawRectangle((int)(e->rect.x + GRID_WIDTH), (int)e->rect.y,
                              (int)(e->rect.width),          (int)e->rect.height, e->color);
                DrawRectangle((int)(e->rect.x - GRID_WIDTH), (int)e->rect.y,
                              (int)(e->rect.width),          (int)e->rect.height, e->color);
            }
        }
        if (e->type == ENTITY_TYPE_LILYPAD)
        {
            DrawCircleV(e->position, e->radius, e->color);
            if (e->wrapping)
            {
                DrawCircle((int)(e->position.x + GRID_WIDTH),
                           (int)e->position.y, e->radius, e->color);
                DrawCircle((int)(e->position.x - GRID_WIDTH),
                           (int)e->position.y, e->radius, e->color);
            }
        }
        if (e->type == ENTITY_TYPE_FROG)
        {
            Color frogColor = (e->dead)? MAROON : e->color;
            DrawCircleV(e->position, e->radius, frogColor);
            if (e->wrapping)
            {
                DrawCircle((int)(e->position.x + GRID_WIDTH),
                           (int)e->position.y, e->radius, frogColor);
                DrawCircle((int)(e->position.x - GRID_WIDTH),
                           (int)e->position.y, e->radius, frogColor);
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
                  (int)(VIRTUAL_WIDTH - (game.gridStart.x + GRID_WIDTH)),
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
    game.frog->dead = true;
    game.deathTimer = 0.5f;
}
