// EXPLANATION:
// All the game logic, including how/when to draw to screen
// See header for more documentation/descriptions

// Initialization
// ----------------------------------------------------------------------------

void InitGameState(ScreenState screen)
{
    game = (GameState){
        .currentScreen = screen,

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

    // Create entities
    // ----------------------------------------------------------------------------
    // Lilypads
    for (int i = 0; i < 5; i++)
    {
        Entity lily = {
            .type = ENTITY_TYPE_LILYPAD,
            .speed = GRID_UNIT*2,
            .radius = GRID_UNIT*.75f,
            .gridIndex = {
                GetRandomValue(0, GRID_RES_X),
                GetRandomValue(2, GRID_RES_Y/4)
            },
            .color = DARKGREEN,
        };
        Vector2 lilyPosition = GetGridPosition(lily.gridIndex.x, lily.gridIndex.y);
        lily.position.x = lilyPosition.x + GRID_UNIT/2;
        lily.position.y = lilyPosition.y + GRID_UNIT/2;
        arrpush(game.entities.lilypads, lily);
    }

    // Frog
    Point spawnPoint = { GRID_RES_X/2, GRID_RES_Y - 2 };
    Entity frog = {
        .type = ENTITY_TYPE_FROG,
        .speed = GRID_UNIT*6.0f,
        .rect.width = GRID_UNIT,
        .rect.height = GRID_UNIT,
        .gridIndex = spawnPoint,
        .color = GREEN,
    };
    Vector2 frogPos = GetGridPosition(frog.gridIndex.x, frog.gridIndex.y);
    frog.rect.x = frogPos.x;
    frog.rect.y = frogPos.y;
    frog.seekPos = frogPos;
    frog.bufferPos = frogPos;
    game.entities.frog = frog;

    // Cars
    Color carColors[3] = { RED, YELLOW, PURPLE };
    for (int i = 0; i < 6; i++)
    {
        Entity car = {
            .type = ENTITY_TYPE_CAR,
            .speed = GRID_UNIT*(GetRandomValue(0, 1) * 2 - 1),
            .rect.width = GRID_UNIT*GetRandomValue(1,2),
            .rect.height = GRID_UNIT*1,
            .gridIndex = {
                GetRandomValue(1, GRID_RES_X - 1),
                GetRandomValue(GRID_RES_Y/2, GRID_RES_Y - 3)
            },
            .color = carColors[i%3],
        };
        Vector2 carPosition = GetGridPosition(car.gridIndex.x, car.gridIndex.y);
        car.rect.x = carPosition.x;
        car.rect.y = carPosition.y;
        arrpush(game.entities.cars, car);
    }
}

void FreeGameState(void)
{
    FreeRaylibAssets(&game.assets);
}

// Update & Draw
// ----------------------------------------------------------------------------

void UpdateGameFrame(void)
{
    game.gridStart = GetGridPosition(0, 0);

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
        for (int i = 0; i < arrlen(game.entities.lilypads); i++)
        {
            Entity *lily = &game.entities.lilypads[i];
            UpdateLilypad(lily);
        }

        Entity *frog = &game.entities.frog;
        UpdateFrog(frog);

        for (int i = 0; i < arrlen(game.entities.cars); i++)
        {
            Entity *car = &game.entities.cars[i];
            UpdateCar(car);
        }
    }
    // Prevent input after resuming pause
    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && game.resumeInputCooldown)
        game.resumeInputCooldown = false;

    // Update user interface elements and logic
    UpdateUiFrame();
}

void UpdateFrog(Entity *frog)
{
    Vector2 frogPos = { frog->rect.x, frog->rect.y };

    // check if frog reached destination
    if (frog->moving && Vector2Equals(frogPos, frog->seekPos))
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
    Vector2 moveVector = Vector2Zero();
    if      (input.player.moveUp)    moveVector.y -= GRID_UNIT;
    else if (input.player.moveDown)  moveVector.y += GRID_UNIT;
    else if (input.player.moveLeft)  moveVector.x -= GRID_UNIT;
    else if (input.player.moveRight) moveVector.x += GRID_UNIT;

    bool moveInput = (input.player.moveUp   || input.player.moveDown ||
                      input.player.moveLeft || input.player.moveRight);

    if (moveInput)
    {
        Vector2 newSeekPos = Vector2Add(frogPos, moveVector);
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
        Vector2 newPos = Vector2MoveTowards(frogPos, frog->seekPos, frog->speed*game.frameTime);
        frog->rect.x = newPos.x;
        frog->rect.y = newPos.y;
    }
}

void UpdateLilypad(Entity *lilypad)
{
    Vector2 frogCenter = {
        game.entities.frog.rect.x + game.entities.frog.rect.width/2,
        game.entities.frog.rect.y + game.entities.frog.rect.height/2
    };
    if (CheckCollisionPointCircle(frogCenter, lilypad->position, lilypad->radius))
    {
        game.entities.frog.rect.x += lilypad->speed*game.frameTime;
        game.entities.frog.seekPos.x += lilypad->speed*game.frameTime;
        game.entities.frog.bufferPos.x += lilypad->speed*game.frameTime;
    }

    if ((lilypad->speed > 0) &&
        (lilypad->position.x > game.gridStart.x + GRID_WIDTH + lilypad->radius))
    {
        lilypad->position.x = game.gridStart.x - lilypad->radius;
    }
    else if (lilypad->position.x < game.gridStart.x - lilypad->radius)
    {
        lilypad->position.x = game.gridStart.x + GRID_WIDTH + lilypad->radius;
    }
    lilypad->position.x += lilypad->speed*game.frameTime;
}

void UpdateCar(Entity *car)
{
    if ((car->speed > 0) &&
        (car->rect.x > game.gridStart.x + GRID_WIDTH))
    {
        car->rect.x = game.gridStart.x - car->rect.width;
    }
    else if (car->rect.x < game.gridStart.x - car->rect.width)
    {
        car->rect.x = game.gridStart.x + GRID_WIDTH;
    }

    car->rect.x += car->speed*game.frameTime;
}

void DrawGameFrame(void)
{
    ClearBackground(DARKBLUE);

    // Draw entities
    for (int i = 0; i < arrlen(game.entities.lilypads); i++)
    {
        Entity *lily = &game.entities.lilypads[i];
        DrawCircleV(lily->position, lily->radius, lily->color);
    }

    Entity *frog = &game.entities.frog;
    DrawRectangleRec(frog->rect, frog->color);

    for (int i = 0; i < arrlen(game.entities.cars); i++)
    {
        Entity *car = &game.entities.cars[i];
        DrawRectangleRec(car->rect, car->color);
    }

    // Cover outside of game grid
    DrawRectangle(-1, 0,
                  (int)game.gridStart.x + 1,
                  (int)VIRTUAL_HEIGHT, BLACK);

    DrawRectangle((int)(game.gridStart.x + GRID_WIDTH),
                  0,
                  (int)(VIRTUAL_WIDTH - (game.gridStart.x + GRID_WIDTH)),
                  (int)VIRTUAL_HEIGHT, BLACK);
}

Vector2 GetGridPosition(int col, int row)
{
    int index = row*GRID_RES_X + col;
    return (Vector2){ game.grid[index].x, game.grid[index].y };
}

