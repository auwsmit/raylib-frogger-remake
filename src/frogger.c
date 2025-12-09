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

        .debugMode = DEBUG_DEFAULT
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
    CreateLogRow(spawnRow++,     "OOO..OOO..OOO...",  GRID_UNIT, 3);
    CreateLilypadRow(spawnRow++, "....OO..OO..OO..", -GRID_UNIT*1.5f);
    CreateLogRow(spawnRow++,     "..OOOOO..OOOOO..",  GRID_UNIT*2, 5);
    CreateLogRow(spawnRow++,     "..OO..OO..OO....",  GRID_UNIT*0.5f, 2);
    CreateLilypadRow(spawnRow++, "OOO.OOO.OOO.OOO.", -GRID_UNIT*1.5f);

    // Frog
    Point spawnPoint = { GRID_RES_X/2, GRID_RES_Y - 1 };
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
    game.entities.frog = frog;
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
        arrpush(game.entities.lilypads, lily);
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
        arrpush(game.entities.logs, log);

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
        arrpush(game.entities.cars, car);

        i += width - 1;
        c += width - 1; // don't make extras when width > 1
    }

    carColorIdx++;
}

// void FreeGameState(void)
// {
//     FreeRaylibAssets(&game.assets);
// }

// Update & Draw
// ----------------------------------------------------------------------------

void UpdateGameFrame(void)
{
    // Pause
    if (input.player.pause || (game.paused && input.menu.cancel))
    {
        static float previousTextFade = 0.0f;
        if (!game.paused)
        {
            game.paused = true;
            ChangeUiMenu(UI_MENU_PAUSE);
            previousTextFade = ui.textFade;
            ui.textFade = 1.0f;
        }
        else
        {
            game.paused = false;
            ui.currentMenu = UI_MENU_NONE;
            ui.textFade = previousTextFade;
        }
        PlaySound(ui.sounds.menu);
    }

    if (!game.paused)
    {
        // Update entities
        game.entities.frog.onPlatform = false;

        for (int i = 0; i < arrlen(game.entities.logs); i++)
        {
            Entity *log = &game.entities.logs[i];
            UpdateEntityPlatform(log);
            UpdateEntityMove(log);
        }

        for (int i = 0; i < arrlen(game.entities.lilypads); i++)
        {
            Entity *lily = &game.entities.lilypads[i];
            UpdateEntityPlatform(lily);
            UpdateEntityMove(lily);
            lily->position.x = lily->rect.x + lily->radius; //TEMP
            lily->position.y = lily->rect.y + lily->radius;
        }

        for (int i = 0; i < arrlen(game.entities.cars); i++)
        {
            Entity *car = &game.entities.cars[i];
            UpdateCarCollide(car);
            UpdateEntityMove(car);
        }

        Entity *frog = &game.entities.frog;
        UpdateFrog(frog);
    }
    // Prevent input after resuming pause
    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && game.resumeInputCooldown)
        game.resumeInputCooldown = false;

    // Update user interface elements and logic
    UpdateUiFrame();
}

void UpdateFrog(Entity *frog)
{
    if (frog->dead)
    {
        game.deathTimer -= game.frameTime;

        if (game.deathTimer < EPSILON)
            SpawnFrog();

        return; // no update
    }

    if (!frog->onPlatform &&
        CheckCollisionPointRec(frog->position, game.background.water))
    {
        FrogDead();
        return;
    }

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
        Vector2 newBufferPos = Vector2Add(frog->seekPos, moveVector);

        // set frog seek position
        if (!frog->moving)
        {
            frog->moving = true;
            frog->seekPos = newSeekPos;
            frog->bufferPos = newSeekPos;
        }
        // set buffered position
        else if (!frog->moveBuffered && !Vector2Equals(frog->bufferPos, newBufferPos))
        {
            frog->bufferPos = newBufferPos;
            frog->moveBuffered = true;
        }
    }

    // move frog
    if (frog->moving)
    {
        Vector2 newPos = Vector2MoveTowards(frog->position, frog->seekPos, frog->speed*game.frameTime);
        frog->position = newPos;
    }
}

void UpdateEntityMove(Entity *e)
{
    if ((e->speed > 0) &&
        (e->rect.x > game.gridStart.x + GRID_WIDTH))
    {
        e->rect.x = game.gridStart.x - e->rect.width;
    }
    else if (e->rect.x < game.gridStart.x - e->rect.width)
    {
        e->rect.x = game.gridStart.x + GRID_WIDTH;
    }

    e->rect.x += e->speed*game.frameTime;
}

void UpdateEntityPlatform(Entity *e)
{
    bool colliding = false;
    if (!game.entities.frog.onPlatform && !game.entities.frog.dead &&
        (colliding = CheckCollisionPointRec(game.entities.frog.position, e->rect)))
        game.entities.frog.onPlatform = true;

    if (colliding && game.entities.frog.onPlatform)
    {
        game.entities.frog.position.x += e->speed*game.frameTime;
        game.entities.frog.seekPos.x += e->speed*game.frameTime;
        game.entities.frog.bufferPos.x += e->speed*game.frameTime;
    }
}

void UpdateCarCollide(Entity *car)
{
    if (!game.entities.frog.dead &&
        CheckCollisionCircleRec(game.entities.frog.position,
                                game.entities.frog.radius/2, car->rect))
    {
        FrogDead();
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
    // Logs
    for (int i = 0; i < arrlen(game.entities.logs); i++)
    {
        Entity *log = &game.entities.logs[i];
        DrawRectangleRec(log->rect, log->color);
    }

    // Lilypads
    for (int i = 0; i < arrlen(game.entities.lilypads); i++)
    {
        Entity *lily = &game.entities.lilypads[i];
        DrawCircleV(lily->position, lily->radius, lily->color);
    }

    // Frog
    Entity *frog = &game.entities.frog;
    Color frogColor = (frog->dead)? MAROON : frog->color;
    DrawCircleV(frog->position, frog->radius, frogColor);

    // Cars
    for (int i = 0; i < arrlen(game.entities.cars); i++)
    {
        Entity *car = &game.entities.cars[i];
        DrawRectangleRec(car->rect, car->color);
    }

    // Draw game border (outside of grid)
    // ----------------------------------------------------------------------------
    DrawRectangle(-1, 0,
                  (int)game.gridStart.x + 1,
                  (int)VIRTUAL_HEIGHT, BLACK);

    DrawRectangle((int)(game.gridStart.x + GRID_WIDTH), 0,
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

void FrogDead(void)
{
    game.entities.frog.dead = true;
    game.deathTimer = 0.5f;
}

void SpawnFrog(void)
{
    game.entities.frog.position = game.spawnPos;
    game.entities.frog.seekPos = game.spawnPos;
    game.entities.frog.bufferPos= game.spawnPos;
    game.entities.frog.dead = false;
}
