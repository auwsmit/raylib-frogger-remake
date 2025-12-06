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
    };

    // Set up grid positions
    for (int row = 0; row < GRID_HEIGHT; row++)
    {
        for (int col = 0; col < GRID_WIDTH; col++)
        {
            int index = row*GRID_WIDTH + col;
            game.grid[index].x = SQUARE_SIZE*col;
            game.grid[index].y = SQUARE_SIZE*row;
        }
    }

    // Create entities
    // ----------------------------------------------------------------------------
    // Lilypads
    for (int i = 0; i < 4; i++)
    {
        Entity lily = {
            .type = ENTITY_TYPE_LILYPAD,
            .speed = 50,
            .radius = SQUARE_SIZE,
            .gridIndex = {
                GetRandomValue(0, GRID_WIDTH),
                GetRandomValue(2, GRID_HEIGHT/4)
            },
            .color = DARKGREEN,
        };
        Vector2 lilyPosition = GetGridPosition(lily.gridIndex.x, lily.gridIndex.y);
        lily.position.x = lilyPosition.x + SQUARE_SIZE/2;
        lily.position.y = lilyPosition.y + SQUARE_SIZE/2;
        arrpush(game.entities.lilypads, lily);
    }

    // Frog
    Point spawnPoint = { GRID_WIDTH/2, GRID_HEIGHT - 2 };
    Entity frog = {
        .type = ENTITY_TYPE_FROG,
        .speed = 250.0f,
        .rect.width = SQUARE_SIZE,
        .rect.height = SQUARE_SIZE,
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
            .speed = 150.0f,
            .rect.width = SQUARE_SIZE*GetRandomValue(1,2),
            .rect.height = SQUARE_SIZE*1,
            .gridIndex = {
                GetRandomValue(1, GRID_WIDTH - 1),
                GetRandomValue(10, GRID_HEIGHT - 3)
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

    // has frog stopped
    if (frog->moving && Vector2Equals(frogPos, frog->seekPos))
    {
        // move to possible buffered position
        if (frog->moveBuffered)
        {
            frog->seekPos = frog->bufferPos;
            frog->moveBuffered = false;
            frog->moving = true;
        }
    }

    // set movement origin point
    Vector2 movePos = (frog->moving)? frog->seekPos : frogPos;

    if (input.player.moveUp)
        movePos.y -= SQUARE_SIZE;
    else if (input.player.moveDown)
        movePos.y += SQUARE_SIZE;
    else if (input.player.moveLeft)
        movePos.x -= SQUARE_SIZE;
    else if (input.player.moveRight)
        movePos.x += SQUARE_SIZE;

    // set frog seek position
    if (!frog->moving && !Vector2Equals(frogPos, movePos))
    {
        {
            frog->moving = true;
            frog->seekPos = movePos;
            frog->bufferPos = movePos;
        }
    }
    // set buffered position
    else if (!frog->moveBuffered && !Vector2Equals(frog->bufferPos, movePos))
    {
        frog->bufferPos = movePos;
        frog->moveBuffered = true;
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
    if (lilypad->position.x < -lilypad->radius)
        lilypad->position.x = VIRTUAL_WIDTH + lilypad->radius;
    lilypad->position.x -= lilypad->speed*game.frameTime;
}

void UpdateCar(Entity *car)
{
    if (car->rect.x > VIRTUAL_WIDTH)
        car->rect.x = -car->rect.width;
    car->rect.x += car->speed*game.frameTime;
}

void DrawGameFrame(void)
{
    ClearBackground(DARKBLUE);
    DrawEntities();
}

void DrawEntities(void)
{
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
}

Vector2 GetGridPosition(int col, int row)
{
    int index = row*GRID_WIDTH + col;
    return (Vector2){ game.grid[index].x, game.grid[index].y };
}

