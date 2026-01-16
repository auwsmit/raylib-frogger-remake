// EXPLANATION:
// All the game logic, including how/when to draw to screen
// See header for more documentation/descriptions

// Initialization
// ----------------------------------------------------------------------------

void InitGameState(void)
{
    game = (GameState){ 0 };
    game.currentScreen = SCREEN_LOGO;

    // Center camera
    game.camera.target = (Vector2){ VIRTUAL_WIDTH/2, VIRTUAL_HEIGHT/2 };
    game.camera.offset = (Vector2){ viewport.renderTexWidth/2, viewport.renderTexHeight/2 };
    game.camera.zoom = viewport.renderTexHeight/VIRTUAL_HEIGHT;

    game.level = 1;
    game.lives = 4;
    game.isDebugMode = DEBUG_DEFAULT;

    game.fly.spawnTimer = GetRandomValue(3, 6);

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

    SetTimedMessage("GAME START!", 80, 2.0f, YELLOW);

    // Textures
    game.textures.atlas = LoadTextureAsset(&game.assets, "assets/textures/frogger.png");
    SetTextureFilter(game.textures.atlas, TEXTURE_FILTER_POINT);
    const float s = SPRITE_SIZE;
    game.textures.car         = (Rectangle){ s*3,    0,      s,   s      };
    game.textures.frog        = (Rectangle){ 0,      0,      s,   s      };
    game.textures.grassPurple = (Rectangle){ s*3,    s*2,    s,   s      };
    game.textures.grassGreen  = (Rectangle){ s*4,    s*1.5f, s,   s*1.5f };
    game.textures.dead        = (Rectangle){ s*3,    s*3,    s,   s      };
    game.textures.dying       = (Rectangle){ 0,      s*3,    s,   s      };
    game.textures.turtle      = (Rectangle){ 0,      s*5,    s,   s      };
    game.textures.turtleSink  = (Rectangle){ s*3,    s*5,    s,   s      };
    game.textures.fly         = (Rectangle){ s*2,    s*6,    s,   s      };
    game.textures.winFrog     = (Rectangle){ s*3,    s*6,    s,   s      };
    game.textures.log         = (Rectangle){ s*6,    s*8,    s,   s      };
    game.textures.life        = (Rectangle){ s*3,    s,      s/2, s/2    };
    game.textures.level       = (Rectangle){ s*3.5f, s,      s/2, s/2    };

    game.font = LoadFont("assets/fonts/PressStart2P.ttf");

    UiText score = { 0 };
    score.text = "SCORE";
    score.fontSize = GRID_UNIT*0.5f,
    score.measure = MeasureTextEx(game.font, score.text, score.fontSize, 0);
    score.position = (Vector2){ game.gridStart.x + GRID_UNIT*2.5f, game.gridStart.y };
    score.color = WHITE;
    UiText scoreNum = score;
    scoreNum.position.y += scoreNum.measure.y;
    scoreNum.position.x = GetGridPosition(3, 0).x;
    ui.score = score;
    ui.scoreNum = scoreNum;

    UiText hiScore = score;
    hiScore.text = "HI-SCORE";
    hiScore.measure = MeasureTextEx(game.font, hiScore.text, hiScore.fontSize, 0);
    hiScore.position = (Vector2){ (VIRTUAL_WIDTH - hiScore.measure.x)/2, game.gridStart.y };
    UiText hiScoreNum = hiScore;
    hiScoreNum.position.y += hiScoreNum.measure.y;
    hiScoreNum.position.x = GetGridPosition(6, 0).x + GRID_UNIT/2;
    ui.hiScore = hiScore;
    ui.hiScoreNum = hiScoreNum;

    // Frog
    Entity frog = {
        .sprite = game.textures.frog,
        .textureOffset = SPRITE_SIZE*2,
        .type = ENTITY_TYPE_FROG,
        .speed = BASE_SPEED*4.0f,
        .radius = GRID_UNIT*0.4f,
        .color = GREEN,
        .animate.sprite = game.textures.dying,
        .animate.frames = 3,
        .animate.offset.x = s,
        .animate.offset.y = s,
        .animate.length = 0.3f
    };
    Vector2 frogSpawnPos = GetGridPosition(8, 14);
    frog.position = frogSpawnPos;
    frog.position.x += GRID_UNIT/2;
    frog.position.y += GRID_UNIT/2 + GRID_UNIT/16;
    arrpush(game.entities, frog);
    game.frog = &arrlast(game.entities);
    game.spawnPos = frog.position;
    game.prevFrogYPos = frog.position.y;

    CreateNextLevel();

    int flyCount = 0;
    for (int i = 0; i < arrlen(game.entities); i++)
    {
        Entity *e = &game.entities[i];
        if (e->type == ENTITY_TYPE_WIN)
        {
            game.fly.entityIdx[flyCount] = i;
            flyCount++;
        }
    }

    // Background rectangles
    game.background.water.x = game.gridStart.x;
    game.background.water.y = game.gridStart.y;
    game.background.water.width = GRID_WIDTH;
    game.background.water.height = GRID_UNIT*8;

    game.background.grassMiddle.x = game.gridStart.x;
    game.background.grassMiddle.y = GetGridPosition(0, 8).y;
    game.background.grassMiddle.width = GRID_WIDTH;
    game.background.grassMiddle.height = GRID_UNIT;

    game.background.grassBottom.x = game.gridStart.x;
    game.background.grassBottom.y = GetGridPosition(0, 14).y;
    game.background.grassBottom.width = GRID_WIDTH;
    game.background.grassBottom.height = GRID_UNIT;
}

void CreateRow(EntityType type, int row, char *pattern, float speed)
{
    const float s = SPRITE_SIZE;
    float carTextureOffsets[5] = {
        s*2, // truck
        s*5, // racer 1
        s*4, // car
        s,   // tractor
        0,   // racer 2
    };
    float carTextureSizes[5] = { s*2, s, s, s, s };
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
            if (*c == 'F' || *c == 'S')
            {
                e.isSinking = true;
                e.animate.sprite = game.textures.turtleSink;
                e.animate.frames = 3;
                e.animate.offset.x = SPRITE_SIZE;
                if (*c == 'F') e.animate.length = 0.5f; // fast sink
                if (*c == 'S') e.animate.length = 1.0f;  // slow sink
            }
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
                e.textureOffset.x = s*2;
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
            e.animate.offset.x = s;
            e.animate.timer = 0.5f*(game.winCount + 1);
            game.winCount++;
        }

        if (type == ENTITY_TYPE_FLY)
        {
            e.sprite = game.textures.fly;
        }

        arrpush(game.entities, e);
    }
}

void CreateNextLevel(void)
{
    game.gameOver = false;
    game.gameWon = false;

    Entity frog = *game.frog;
    if (game.entities) arrfree(game.entities);

    float speed = BASE_SPEED;
    if (game.level > 1) speed *= game.level*0.7f; // TEMP until more level layouts

    // Win zones
    int spawnRow = 2;
    CreateRow(ENTITY_TYPE_WALL,   spawnRow,   ".O_OO_OO_OO_OO_O.", 0);
    CreateRow(ENTITY_TYPE_WIN,    spawnRow,   "._O__O__O__O__O_.",  0);
    CreateRow(ENTITY_TYPE_FLY,    spawnRow,   "._O__O__O__O__O_.",  0);

    // River (logs, turtles, etc)
    CreateRow(ENTITY_TYPE_LOG,    ++spawnRow, "_OOOO_.OOOO_.OOOO", speed*0.8f);
    CreateRow(ENTITY_TYPE_TURTLE, ++spawnRow, "___SS_.OO_.OO_.OO", -speed);
    CreateRow(ENTITY_TYPE_LOG,    ++spawnRow, "__OOOOOO__OOOOOO",  speed*2);
    CreateRow(ENTITY_TYPE_LOG,    ++spawnRow, "___OOO__OOO__OOO",  speed*0.5f);
    CreateRow(ENTITY_TYPE_TURTLE, ++spawnRow, "_FFF_OOO_OOO_OOO",  -speed);

    // Road, Cars
    spawnRow = 9;
    CreateRow(ENTITY_TYPE_CAR, spawnRow,   "________.OO___.OO", -speed);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "O_______________",  speed*0.6f);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "_______O___O___O",  -speed*0.6f);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "_______O___O___O",  speed*0.4f);
    CreateRow(ENTITY_TYPE_CAR, ++spawnRow, "______O___.O___.O", -speed*0.4f);

    arrpush(game.entities, frog);
    game.frog = &arrlast(game.entities);
    RespawnFrog();
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
        if ((game.winCount == 0) && !game.gameWon)
        {
            game.gameWon = true;
            SetTimedMessage("WINNER!", 100, 3.0f, YELLOW);
            game.waitTimer = 3.5f;
        }
        if (game.gameWon && (game.waitTimer < EPSILON))
        {
            game.level++;
            CreateNextLevel();
        }

        // Update score
        if (game.score > game.hiScore)
            game.hiScore = game.score;
        ui.scoreNum.text = TextFormat("%i", game.score);
        ui.hiScoreNum.text = TextFormat("%i", game.hiScore);

        // Game over condition
        if ((game.lives == 0) && !game.gameOver)
        {
            game.gameOver = true;
            SetTimedMessage("GAME OVER!", 80, 2.0f, RED);
            game.waitTimer = 3.0f;
        }
        if (game.gameOver && (game.waitTimer < EPSILON))
        {
            game.level = 1;
            game.score = 0;
            game.lives = 4;
            CreateNextLevel();
        }

        // Update entities
        UpdateFrog();

        for (int i = 0; i < arrlen(game.entities); i++)
        {
            Entity *e = &game.entities[i];

            if (e->type == ENTITY_TYPE_WIN)      UpdateWinZone(e, i);
            if (e->flags & ENTITY_FLAG_KILL)     UpdateHostile(e);
            if (e->flags & ENTITY_FLAG_PLATFORM) UpdatePlatform(e);
            if (e->flags & ENTITY_FLAG_MOVE)     MoveEntity(e);
            if (e->isSinking)                    UpdateSinkingTurtle(e);
        }

        // Update flies
        if (game.fly.idx == 0)
        {
            if (game.fly.spawnTimer < EPSILON)
            {
                game.fly.spawnTimer = (float)GetRandomValue(1, 10);
                game.fly.despawnTimer = 3;
                game.fly.idx = GetRandomValue(0, 5);
            }
            else
                game.fly.spawnTimer -= game.frameTime;
        }
        else
        {
            if (game.fly.despawnTimer > EPSILON)
                game.fly.despawnTimer -= game.frameTime;
            else
            {
                game.fly.idx = 0;
            }
        }

        // global game wait timer (player cannot move)
        if (game.waitTimer > 0)
            game.waitTimer -= game.frameTime;

        // Update global turtle animation
        if (game.animateTimer > 0)
            game.animateTimer -= game.frameTime;
        else
        {
            game.animateTimer = 0.25f;
            game.animateTextureOffset = fmodf(game.animateTextureOffset + game.textures.turtle.width, SPRITE_SIZE*3);
        }
    }
    // Prevent input after resuming pause
    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && game.isInputDisabledFromResume)
        game.isInputDisabledFromResume = false;

    // Update user interface elements and logic
    UpdateUiFrame();
}

void UpdateFrog(void)
{
    // track to moving platform
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
        // update death animation
        if ((game.frog->animate.timer < EPSILON) &&
            (game.frog->animate.frame <= game.frog->animate.frames))
        {
            game.frog->animate.frame++;
            if (game.frog->animate.frame >= 2)
                game.frog->animate.timer /= game.frog->animate.frame;
            game.frog->animate.timer = game.frog->animate.length;
        }
        else game.frog->animate.timer -= game.frameTime;
        game.frog->textureOffset.x = game.frog->animate.offset.x*(game.frog->animate.frame - 1);


        game.deathTimer -= game.frameTime;

        if ((game.deathTimer < 0) && !game.gameOver)
            RespawnFrog();

        return; // no update
    }

    // drowned in river (lethal rapids, I guess?)
    if (!game.frog->isOnPlatform &&
        CheckCollisionPointRec(game.frog->position, game.background.water))
    {
        KillFrog();
        game.frog->isDrowned = true;
        game.frog->textureOffset.y = 0; // set to drown death animation
        return;
    }
    game.frog->isOnPlatform = false;

    // frog reached next seek position
    if (game.frog->isMoving && Vector2Equals(game.frog->position, game.frog->seekPos))
    {
        // +10 points for moving forward
        if (game.frog->position.y < game.prevFrogYPos)
        {
            game.prevFrogYPos = game.frog->position.y;
            game.rowsTravelled++;
            game.score += 10;
        }

        // move to possible buffered position
        if (game.frog->isMoveBuffered)
        {
            game.frog->seekPos = game.frog->bufferPos;
            game.frog->isMoveBuffered = false;
        }
        else game.frog->isMoving = false;
    }

    if ((game.lives == 0)) return;

    // set movement vector
    bool moveInput = (input.player.moveUp   || input.player.moveDown ||
                      input.player.moveLeft || input.player.moveRight);

    if (moveInput && (game.waitTimer < EPSILON))
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
        bool pastBottomEdge = (newSeekPos.y - game.frog->radius > game.gridStart.y + GRID_HEIGHT - GRID_UNIT);
        if (pastLeftEdge || pastRightEdge || pastBottomEdge) return;

        Vector2 newBufferPos = Vector2Add(game.frog->seekPos, moveVector);

        // set game.frog seek position
        if (!game.frog->isMoving)
        {
            game.frog->isMoving = true;
            game.frog->seekPos = newSeekPos;
        }

        // set buffered position
        else if (!game.frog->isMoveBuffered && !Vector2Equals(game.frog->bufferPos, newBufferPos))
        {
            pastLeftEdge   = (newBufferPos.x + game.frog->radius < game.gridStart.x + GRID_UNIT);
            pastRightEdge  = (newBufferPos.x - game.frog->radius > game.gridStart.x + GRID_WIDTH - GRID_UNIT);
            pastBottomEdge = (newBufferPos.y - game.frog->radius > game.gridStart.y + GRID_HEIGHT - GRID_UNIT);
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
            game.frog->textureOffset.x = SPRITE_SIZE*2; // not hopping
        else
            game.frog->textureOffset.x = 0; // hopping

        if (moveDelta.x > 0) game.frog->angle = 270;
        if (moveDelta.x < 0) game.frog->angle = 90;
        if (moveDelta.y > 0) game.frog->angle = 0;
        if (moveDelta.y < 0) game.frog->angle = 180;
    }
    else game.frog->textureOffset.x = SPRITE_SIZE*2;
}

void UpdateSinkingTurtle(Entity *turtle)
{
    if (turtle->animate.timer < EPSILON)
    {
        turtle->animate.frame += turtle->animate.frameIterate;
        turtle->animate.timer = turtle->animate.length;
        if (turtle->animate.frame >= 2)
            turtle->animate.timer /= turtle->animate.frame;

        if (turtle->animate.frame == turtle->animate.frames)
            turtle->animate.frameIterate = -1;
        else if (turtle->animate.frame == 0)
            turtle->animate.frameIterate = 1;

    }
    else
        turtle->animate.timer -= game.frameTime;

    turtle->textureOffset.x = (float)((turtle->animate.frame - 1)*turtle->animate.offset.x);
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
        if (platform->isSinking && platform->animate.frame == 3)
            game.frog->isOnPlatform = false; // for sinking turtles
    }

    if (colliding && game.frog->isOnPlatform)
    {
        game.frog->platformMove = platform->speed;
    }
}

void UpdateWinZone(Entity *zone, int entityIndex)
{
    if (!zone->isWin && CheckCollisionPointRec(game.frog->position, zone->rec))
    {
        game.score += 50; // win score
        if ((game.fly.idx > 0) && (game.fly.entityIdx[game.fly.idx - 1] == entityIndex))
            game.score += 200; // fly score
        zone->isWin = true;
        zone->flags |= ENTITY_FLAG_KILL;
        game.winCount--;
        RespawnFrog();
    }

    if (game.gameWon)
    {
        if (zone->animate.timer < EPSILON)
        {
            zone->textureOffset.x = zone->animate.offset.x;
            game.winIndex++;
        }
        else zone->animate.timer -= game.frameTime;
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
    ClearBackground(BLACK);

    // Draw background elements
    DrawRectangleRec(game.background.water, WATER_COLOR);
    DrawGrass(game.background.grassMiddle);
    DrawGrass(game.background.grassBottom);


    // Draw entities
    // ----------------------------------------------------------------------------
    const float s = SPRITE_SIZE;

    for (int i = 0; i < arrlen(game.entities); i++)
    {
        Entity *e = &game.entities[i];
        Rectangle sprite = e->sprite;
        sprite.x += e->textureOffset.x;
        sprite.y += e->textureOffset.y;

        // Draw grass on top of screen
        if (e->type == ENTITY_TYPE_WALL)
            DrawSpriteOnRectangle(&game.textures.atlas, sprite, e->rec, e->angle);

        // Win zones
        if (e->type == ENTITY_TYPE_WIN)
        {
            Rectangle topGrass = game.textures.grassGreen;
            topGrass.x += s;
            topGrass.height -= s/2;
            Rectangle grassRec = e->rec;
            grassRec.y -= GRID_UNIT/2;
            DrawSpriteOnRectangle(&game.textures.atlas, topGrass, grassRec, e->angle);
            if (e->isWin)
                DrawSpriteOnRectangle(&game.textures.atlas, sprite, e->rec, e->angle);
            else if ((game.fly.idx > 0) && (game.fly.entityIdx[game.fly.idx - 1] == i))
                DrawSpriteOnRectangle(&game.textures.atlas, game.textures.fly, e->rec, e->angle);
        }

        if (e->type == ENTITY_TYPE_CAR || e->type == ENTITY_TYPE_TURTLE)
        {
            if (e->type == ENTITY_TYPE_TURTLE)
            {
                if (e->animate.frame > 0)
                {
                    sprite = e->animate.sprite;
                    sprite.x += e->textureOffset.x;
                }
                else sprite.x = e->sprite.x + game.animateTextureOffset;
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
            wrapLeftRec.width = GRID_UNIT;
            wrapLeftRec.x += GRID_WIDTH;
            Rectangle wrapRightRec = e->rec;
            wrapRightRec.width = GRID_UNIT;
            wrapRightRec.x -= GRID_WIDTH;

            Rectangle rec = e->rec;
            rec.width = GRID_UNIT;
            float logWidth = e->rec.width/GRID_UNIT;
            for (int i = 1; i <= logWidth; i++)
            {
                if (i > 1)
                    sprite.x = e->sprite.x + s; // log middle
                if (i == logWidth)
                    sprite.x = e->sprite.x + s*2; // log end
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
        if ((e->type == ENTITY_TYPE_FROG) && !game.gameWon)
        {
            float angle;
            if (e->isDead)
            {
                if (e->animate.frame > e->animate.frames)
                    sprite = game.textures.dead;
                else
                {
                    sprite = e->animate.sprite;
                    sprite.x += e->textureOffset.x;
                    sprite.y += e->textureOffset.y;
                }
                angle = 0;
            }
            else angle = e->angle;

            Vector2 frogPos = e->position;

            DrawSpriteOnCircle(&game.textures.atlas, sprite, frogPos, GRID_UNIT/2, angle);
            if (e->isWrapping)
            {
                Vector2 wrapLeftPos = { frogPos.x + GRID_WIDTH, frogPos.y };
                Vector2 wrapRightPos = { frogPos.x - GRID_WIDTH, frogPos.y };
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
                   BG_COLOR);
    DrawRectangleV((Vector2){ game.gridStart.x + GRID_WIDTH - GRID_UNIT, 0 },
                   (Vector2){ VIRTUAL_WIDTH - GRID_WIDTH + GRID_UNIT, VIRTUAL_HEIGHT },
                   BG_COLOR);
    DrawRectangleV((Vector2){ game.gridStart.x, 0 },
                   (Vector2){ GRID_WIDTH, (VIRTUAL_HEIGHT + GRID_UNIT - (game.gridStart.y + GRID_HEIGHT)) },
                   BG_COLOR);
    DrawRectangleV((Vector2){ game.gridStart.x, (game.gridStart.y + GRID_HEIGHT - GRID_UNIT) },
                   (Vector2){ GRID_WIDTH, (VIRTUAL_HEIGHT + GRID_UNIT - (game.gridStart.y + GRID_HEIGHT)) },
                   BG_COLOR);
    Rectangle innerBorder = {
        game.gridStart.x - GRID_UNIT/2, game.gridStart.y - GRID_UNIT/2,
        GRID_WIDTH + GRID_UNIT, GRID_WIDTH + GRID_UNIT,
    };
    DrawRectangleLinesEx(innerBorder, GRID_UNIT/2, DARKGREEN);

    // Draw HUD
    // ----------------------------------------------------------------------------
    DrawUiText(game.font, ui.score);
    DrawUiText(game.font, ui.scoreNum);
    DrawUiText(game.font, ui.hiScore);
    DrawUiText(game.font, ui.hiScoreNum);

    Vector2 lifePos = game.gridStart;
    lifePos.x += GRID_UNIT;
    lifePos.y += GRID_HEIGHT - GRID_UNIT;
    Rectangle lifeRec = { lifePos.x, lifePos.y, GRID_UNIT/2, GRID_UNIT/2 };
    for (int i = 0; i < game.lives; i++)
    {
        DrawSpriteOnRectangle(&game.textures.atlas, game.textures.life, lifeRec, 0);
        lifeRec.x += GRID_UNIT/2;
    }

    Vector2 levelPos = game.gridStart;
    levelPos.x += GRID_WIDTH - GRID_UNIT*2;
    levelPos.y += GRID_HEIGHT - GRID_UNIT;
    Rectangle levelRec = { levelPos.x, levelPos.y, GRID_UNIT/2, GRID_UNIT/2 };
    for (int i = 0; i < game.level; i++)
    {
        DrawSpriteOnRectangle(&game.textures.atlas, game.textures.level, levelRec, 0);
        levelRec.x -= GRID_UNIT/2;
    }
}

void DrawGrass(Rectangle grassRec)
{
    int tileAmount = (int)(grassRec.width/GRID_UNIT);
    grassRec.width = GRID_UNIT + 0.1f;

    for (int i = 0; i < tileAmount; i++)
    {
        DrawSpriteOnRectangle(&game.textures.atlas, game.textures.grassPurple, grassRec, 0);
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
    game.frog->animate.frame = 0;
    game.deathTimer = 1.5f;
    game.frog->textureOffset.x = 0;
    game.frog->textureOffset.y = game.frog->animate.offset.y; // default land death animation
    game.lives--;
}

void RespawnFrog(void)
{
    game.frog->position = game.spawnPos;
    game.frog->seekPos = game.spawnPos;
    game.frog->bufferPos= game.spawnPos;
    game.frog->isWin = false;
    game.frog->isDead = false;
    game.frog->isDrowned = false;
    game.frog->textureOffset.y = 0;
    game.frog->textureOffset.x = 0;
    game.prevFrogYPos = game.spawnPos.y;
    game.rowsTravelled = 0;
}
