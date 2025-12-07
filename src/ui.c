// EXPLANATION:
// For managing the user interface
// See header for more documentation/descriptions

// Initialize
// ----------------------------------------------------------------------------

void InitUiState(void)
{
    ui = (UiState){
        .camera.target = (Vector2){ VIRTUAL_WIDTH/2, VIRTUAL_HEIGHT/2 },
        .currentMenu = UI_MENU_TITLE,
        .selectedId = 0,
        .mouseHoverId = -1,
        .firstFrame = true,
    };

    // Title menu
    // ----------------------------------------------------------------------------
    ui.initMenu = UI_MENU_TITLE; // new buttons are added to this menu
    UiMenu *titleMenu = &ui.menus[UI_MENU_TITLE];
    UiSelectionStyle selectionStyleFlags = (UI_SELSTYLE_HL_RECT | UI_SELSTYLE_HL_TEXT);
    // UiSelectionStyle selectionStyleFlags = (UI_SELSTYLE_GROW | UI_SELSTYLE_HL_TEXT);
    titleMenu->buttonWidth = UI_TITLE_BUTTON_WIDTH;
    titleMenu->fontSize = UI_MENU_FONT_SIZE;
    titleMenu->spacing = UI_BUTTON_SPACING;
    titleMenu->selectStyleFlags = selectionStyleFlags;

    SetUiAlignMode(UI_ALIGN_CENTER, UI_ALIGN_TOP);
    float alignOffsetY = UI_TITLE_TOP_PADDING + PLATFORM_TITLE_PADDING;
    CreateUiText("Frogger", 0, alignOffsetY, UI_TITLE_FONT_SIZE);
    CreateUiText("Remake", 0, alignOffsetY + UI_TITLE_FONT_SIZE + 10, UI_TITLE_FONT_SIZE);

    SetUiAlignMode(UI_ALIGN_CENTER, UI_ALIGN_MIDDLE);
    CreateUiMenuButton("Start", UiCallbackStartGame, 0, 30);
    CreateUiMenuButtonRelative("Settings", UiCallbackSettings);
    if (PLATFORM_CAN_EXIT)
        CreateUiMenuButtonRelative("Exit", UiCallbackExit);

    // Settings menu
    // ----------------------------------------------------------------------------
    ui.initMenu = UI_MENU_SETTINGS;
    UiMenu *settingsMenu = &ui.menus[UI_MENU_SETTINGS];
    settingsMenu->buttonWidth = UI_SETTINGS_BUTTON_WIDTH;
    settingsMenu->fontSize = UI_SETTINGS_FONT_SIZE;
    settingsMenu->spacing = UI_BUTTON_SPACING;
    settingsMenu->selectStyleFlags = selectionStyleFlags;

    SetUiAlignMode(UI_ALIGN_CENTER, UI_ALIGN_TOP);
    CreateUiText("SETTINGS", 0, VIRTUAL_HEIGHT*0.15f, UI_TITLE_FONT_SIZE);
    CreateUiMenuButton("Fullscreen:", UiCallbackToggleFullscreen, 0, VIRTUAL_HEIGHT*0.35f);
    CreateUiCheckbox(UiCallbackCheckFullscreen);
    CreateUiMenuButtonRelative("Volume:", 0);
    CreateUiSlider(UiCallbackSetVolume, GetMasterVolume, 0.0f, 1.0f, 0.1f);
    CreateUiMenuButtonRelative("Back", UiCallbackGoBack);

    // Pause menu
    // ----------------------------------------------------------------------------
    ui.initMenu = UI_MENU_PAUSE;
    UiMenu *pauseMenu = &ui.menus[UI_MENU_PAUSE];
    pauseMenu->buttonWidth = UI_PAUSE_BUTTON_WIDTH;
    pauseMenu->fontSize = UI_MENU_FONT_SIZE;
    pauseMenu->spacing = UI_BUTTON_SPACING;
    pauseMenu->selectStyleFlags = selectionStyleFlags;

    SetUiAlignMode(UI_ALIGN_CENTER, UI_ALIGN_TOP);
    CreateUiText("PAUSED", 0, VIRTUAL_HEIGHT*0.15f, UI_TITLE_FONT_SIZE);
    CreateUiMenuButton("Resume", UiCallbackResume, 0, VIRTUAL_HEIGHT*0.4f);
    CreateUiMenuButtonRelative("Settings", UiCallbackSettings);
    CreateUiMenuButtonRelative("Title Screen", UiCallbackGoToTitle);
    if (PLATFORM_CAN_EXIT)
        CreateUiMenuButtonRelative("Exit Game", UiCallbackExit);

    // Sound assets
    ui.sounds.menu =  LoadSoundAsset(&ui.assets, "assets/audio/menu_beep.wav");

    // Touch input buttons (virtual gamepad)
    // ----------------------------------------------------------------------------
    // Analog stick
    UiAnalogStick stick = { 0 };
    stick.centerPos.x = UI_STICK_RADIUS + UI_INPUT_PADDING;
    stick.centerPos.y = VIRTUAL_HEIGHT - UI_STICK_RADIUS - UI_INPUT_PADDING;
    stick.stickPos = stick.centerPos;
    stick.centerRadius = UI_STICK_RADIUS;
    stick.stickRadius = UI_STICK_RADIUS/2;
    stick.lastTouchId = -1;
    stick.enabled = false;
    ui.gamepad.stick = stick;

    // D-Pad
    UiDPad dpad = { 0 };
    dpad.width = UI_DPAD_WIDTH;
    dpad.position.x = dpad.width*1.5f + UI_INPUT_PADDING;
    dpad.position.y = VIRTUAL_HEIGHT - dpad.width*1.5f - UI_INPUT_PADDING;
    dpad.button[UI_DPAD_UP] =
        (Rectangle){ dpad.position.x - dpad.width*1.5f,
            dpad.position.y - dpad.width*1.5f,
            dpad.width*3, dpad.width };
    dpad.button[UI_DPAD_DOWN] =
        (Rectangle){ dpad.position.x - dpad.width*1.5f,
            dpad.position.y + dpad.width*0.5f,
            dpad.width*3, dpad.width };
    dpad.button[UI_DPAD_LEFT] =
        (Rectangle){ dpad.position.x - dpad.width*1.5f,
            dpad.position.y - dpad.width*1.5f,
            dpad.width, dpad.width*3 };
    dpad.button[UI_DPAD_RIGHT] =
        (Rectangle){ dpad.position.x + dpad.width*0.5f,
            dpad.position.y - dpad.width*1.5f,
            dpad.width, dpad.width*3 };
    dpad.rect =
        (Rectangle){ dpad.button[UI_DPAD_UP].x, dpad.button[UI_DPAD_UP].y,
            dpad.width*3, dpad.width*3 };
    dpad.inputActionId[UI_DPAD_UP] = INPUT_ACTION_UP;
    dpad.inputActionId[UI_DPAD_DOWN] = INPUT_ACTION_DOWN;
    dpad.inputActionId[UI_DPAD_LEFT] = INPUT_ACTION_LEFT;
    dpad.inputActionId[UI_DPAD_RIGHT] = INPUT_ACTION_RIGHT;
    dpad.enabled = true;
    ui.gamepad.dpad = dpad;

    // Pause button
    float pausePosX = VIRTUAL_WIDTH/3;
    float pausePosY = VIRTUAL_HEIGHT - UI_INPUT_RADIUS*0.8f - UI_INPUT_PADDING;
    ui.gamepad.pause = InitUiInputButton("Pause", INPUT_ACTION_PAUSE, pausePosX, pausePosY, UI_INPUT_RADIUS*0.8f);

    ui.gamepad.stick.textureBase = LoadTextureAsset(&ui.assets, "assets/textures/analog_stick_base.png");
    ui.gamepad.stick.textureNub  = LoadTextureAsset(&ui.assets, "assets/textures/analog_stick_nub.png");
    ui.gamepad.dpad.texture      = LoadTextureAsset(&ui.assets, "assets/textures/dpad.png");
    ui.gamepad.a.texture         = LoadTextureAsset(&ui.assets, "assets/textures/button_a.png");
    ui.gamepad.x.texture         = LoadTextureAsset(&ui.assets, "assets/textures/button_x.png");
    ui.gamepad.pause.texture     = LoadTextureAsset(&ui.assets, "assets/textures/button_pause.png");
}

UiButton InitUiButton(char *text, UiActionFunc actionFunc, float x, float y, float buttonWidth, int fontSize)
{
    int textWidth = MeasureText(text, fontSize);
    int growWidth = MeasureText(text, (int)(fontSize*UI_SELECT_GROWTH_MULT));
    float growPosX = x;
    if (buttonWidth == 0) buttonWidth = (float)textWidth + 25;

    float buttonHeight = fontSize + fontSize*0.2f;
    if (ui.hAlign != UI_ALIGN_DISABLED)
    {
        x += Lerp(0.0f, (float)VIRTUAL_WIDTH - buttonWidth, ((float)ui.hAlign)*0.5f);
        growPosX += Lerp(0.0f, (float)VIRTUAL_WIDTH - growWidth, ((float)ui.hAlign)*0.5f);
    }
    if (ui.vAlign != UI_ALIGN_DISABLED)
        y += Lerp(0.0f, (float)VIRTUAL_HEIGHT - buttonHeight, ((float)ui.vAlign)*0.5f);

    Vector2 textPos;
    if (buttonWidth == (float)textWidth)
        textPos = (Vector2){ x, y };
    else textPos = (Vector2){ x + buttonWidth/2 - textWidth/2, y };

    UiButton button = {
        .text = text,
        .onClick = actionFunc,
        .height = buttonHeight,
        .width = buttonWidth,
        .fontSize = fontSize,
        .rect = (Rectangle){
            x, y - buttonHeight*0.1f,
            buttonWidth, buttonHeight
        },
        .position = textPos,
        .growPos = (Vector2){ growPosX, y - (fontSize*UI_SELECT_GROWTH_MULT - fontSize)/2 },
        .bordered = UI_BORDERED_DEFAULT,
        .color = RAYWHITE
    };

    return button;
}

void CreateUiText(char *text, float x, float y, int fontSize)
{
    if (ui.hAlign != UI_ALIGN_DISABLED)
    {
        int textLength = MeasureText(text, fontSize);
        x += Lerp(0.0f, (float)VIRTUAL_WIDTH - textLength, ((float)ui.hAlign)*0.5f);
    }
    if (ui.vAlign != UI_ALIGN_DISABLED)
        y += Lerp(0.0f, (float)VIRTUAL_HEIGHT - fontSize, ((float)ui.vAlign)*0.5f);

    UiText textElement = {
        .text = text,
        .position = { x, y },
        .fontSize = fontSize,
        .color = RAYWHITE
    };

    arrput(ui.menus[ui.initMenu].text, textElement);
}

UiButton *CreateUiMenuButton(char *text, UiActionFunc actionFunc, float x, float y)
{
    UiMenu *menu = &ui.menus[ui.initMenu];
    UiButton button = InitUiButton(text, actionFunc, x, y, menu->buttonWidth, menu->fontSize);
    arrput(menu->buttons, button);
    ui.lastButtonCreated = &arrlast(menu->buttons);

    return &menu->buttons[arrlen(menu->buttons) - 1];
}

void CreateUiMenuButtonRelative(char* text, UiActionFunc actionFunc)
{
    UiMenu *menu = &ui.menus[ui.initMenu];
    UiButton *originButton = &menu->buttons[arrlen(menu->buttons) - 1];
    UiButton *button = CreateUiMenuButton(text, actionFunc, 0, 0);
    if (ui.hAlign == UI_ALIGN_DISABLED)
    {
        int textLength = MeasureText(text, menu->fontSize);
        button->rect.x = originButton->rect.x;
        button->position.x = button->rect.x + button->width/2 - textLength/2;
    }
    button->rect.y = originButton->rect.y + originButton->height + menu->spacing;
    button->position.y = originButton->position.y + originButton->height + menu->spacing;
    button->growPos.y = originButton->growPos.y + originButton->height + menu->spacing;
}

UiButton InitUiInputButton(char *text, int inputActionId, float x, float y, float radius)
{
    UiButton button = {
        .text = text,
        .textureScale = 1.333f,
        .radius = radius,
        .inputActionId = inputActionId,
        .position = { x, y },
        .color = RAYWHITE
    };

    return button;
}

void CreateUiCheckbox(UiGetBoolFunc getValue)
{
    UiButton *button = ui.lastButtonCreated;
    float radius = button->height/2 - 10;

    UiCheckbox newCheckbox = {
        .radius = radius,
        .getValue = getValue,
    };

    // reposition text with checkbox
    button->position.x -= radius*0.75f + 25;
    button->growPos.x -= radius*0.75f + 25;
    int textLength = MeasureText(button->text, button->fontSize);
    int growLength = MeasureText(button->text, (int)(button->fontSize*UI_SELECT_GROWTH_MULT));
    newCheckbox.position =
        (Vector2){
            button->position.x + textLength + radius + 25,
            button->rect.y + button->height/2,
        };
    newCheckbox.growPos =
        (Vector2){
            button->growPos.x + growLength + radius + 25,
            button->rect.y + button->height/2,
        };

    // reposition rect if necessary
    if (ui.menus[ui.initMenu].buttonWidth == 0)
    {
        button->rect.width += radius*2 + 25;
        button->rect.x -= radius*0.75f + 25;
    }

    button->checkbox = malloc(sizeof(UiCheckbox));
    *button->checkbox = newCheckbox;
}

void CreateUiSlider(UiSetFloatFunc setValue, UiGetFloatFunc getValue, float minValue, float maxValue, float increment)
{
    UiButton *button = ui.lastButtonCreated;

    UiSlider newSlider = {
        .rect = (Rectangle){
            button->rect.x + UI_BORDER_THICKNESS*3,
            button->rect.y + button->height,
            button->width - UI_BORDER_THICKNESS*6, button->height/2 - UI_BORDER_THICKNESS*3
        },
        .min = minValue, .max = maxValue,
        .setValue = setValue,
        .getValue = getValue,
        .increment = increment
    };

    // resize button
    button->rect.height += button->height/2;
    button->height += button->height/2;

    button->slider = malloc(sizeof(UiSlider));
    *button->slider = newSlider;
}

void SetUiAlignMode(UiAlignment hAlign, UiAlignment vAlign)
{
    ui.hAlign = hAlign;
    ui.vAlign = vAlign;
}

// void DisableAlignMode(void)
// {
//     ui.hAlign = UI_ALIGN_DISABLED;
//     ui.vAlign = UI_ALIGN_DISABLED;
// }

void FreeUiState(void)
{
    for (int i = 0; i < UI_MENU_AMOUNT; i++)
        arrfree(ui.menus[i].buttons);

    FreeRaylibAssets(&ui.assets);
}

// Update / User Input
// ----------------------------------------------------------------------------

void UpdateUiFrame(void)
{
    if (input.global.debug)
        game.debugMode = !game.debugMode;

    // Update text fade animation
    static float fadeLength = 1.5f; // Fade in and out at this rate in seconds
    static bool fadingOut = false;
    float fadeIncrement = (1.0f/fadeLength)*game.frameTime;

    if (ui.textFade >= 1.0f)
        fadingOut = true;
    else if (ui.textFade <= 0.0f)
        fadingOut = false;
    if (fadingOut)
        fadeIncrement *= -1;

    // Update volume playback timer
    if (ui.playbackTimer > EPSILON) ui.playbackTimer -= game.frameTime;

    ui.textFade += fadeIncrement;

    // Update title menu
    if (ui.currentMenu != UI_MENU_NONE)
    {
        // Cancel/Back to main title menu
        if (input.menu.cancel &&
            ui.currentMenu != UI_MENU_TITLE &&
            ui.currentMenu != UI_MENU_PAUSE)
        {
            ChangeUiMenu(UI_MENU_TITLE);
            PlaySound(ui.sounds.menu);
        }

        // Input for menu selection and movement
        UpdateUiMenuTraverse();
        UiButton *selectedButton = &ui.menus[ui.currentMenu].buttons[ui.selectedId];
        UpdateUiButtonSelect(selectedButton);
    }

    // Update specific text fade
    if (game.paused)
    {
        UiText *pauseText = &ui.menus[UI_MENU_PAUSE].text[0];
        pauseText->color = Fade(RAYWHITE, ui.textFade);
        UiText *settingsText = &ui.menus[UI_MENU_SETTINGS].text[0];
        settingsText->color = Fade(RAYWHITE, ui.textFade);
    }
}

void UpdateUiMenuTraverse(void)
{
    if (ui.currentMenu == UI_MENU_NONE) return; // no menu to update

    UiMenu *menu = &ui.menus[ui.currentMenu];

    int prevId = ui.selectedId; // used to determine when to play a sound
    bool newMouseHover = false;
    bool sliderActive = ((ui.selectedId != -1) &&
                         menu->buttons[ui.selectedId].slider &&
                         menu->buttons[ui.selectedId].slider->active);

    // Deselect when not touching for touch screen
    if (input.touchMode && input.touchCount == 0)
    {
        ui.selectedId = -1;
        ui.mouseHoverId = -1;
    }

    // Keep slider button selected while active
    else if (sliderActive); // don't update traversal

    // Move cursor via mouse (also works for first touch point)
    else if (input.mouse.moved || (ui.firstFrame && ui.lastSelectWithMouse))
    {
        bool buttonFound = false;
        for (int i = 0; i < arrlen(menu->buttons); i++)
        {
            UiButton *currentButton = 0;
            currentButton = &menu->buttons[i];

            if (IsMouseWithinUiButton(currentButton))
            {
                buttonFound = true;
                ui.selectedId = i;
                if (ui.mouseHoverId == -1)
                    newMouseHover = true;
                ui.mouseHoverId = i;
                ui.lastSelectWithMouse = true;
                break;
            }
        }
        if (!buttonFound)
        {
            ui.mouseHoverId = -1;
            ui.selectedId = -1;
            prevId = -1;
        }
    }

    // Move cursor via input actions
    static AutoRepeatSettings autoRepeat = { .triggerTime = 0.6f, .fireInterval = 0.1f };
    bool validInput = (input.menu.moveUp || input.menu.moveDown);
    if (AutoRepeatShouldFire(&autoRepeat, validInput))
    {
        if (input.menu.moveUp)
        {
            if (ui.selectedId == -1) ui.selectedId = (int)arrlen(menu->buttons); // up defaults to last menu item
            ui.selectedId = ((ui.selectedId + (int)arrlen(menu->buttons) - 1) % arrlen(menu->buttons));
            ui.lastSelectWithMouse = false;
        }
        if (input.menu.moveDown)
        {
            ui.selectedId = ((ui.selectedId + 1) % arrlen(menu->buttons)); // down defaults to first menu item (-1 + 1)
            ui.lastSelectWithMouse = false;
        }
    }

    // Play sound when new item selected
    bool cursorMoved = (ui.selectedId != (int)prevId);

    if ((cursorMoved || newMouseHover) && !ui.firstFrame && !input.touchMode)
        PlaySound(ui.sounds.menu);

    ui.firstFrame = false;
}

void UpdateUiButtonSelect(UiButton *button)
{
    if (ui.selectedId == -1) return; // nothing selected

    // int touchIdx = IsTouchWithinUiButton(button);
    // bool buttonTapped = ((touchIdx != -1) && IsTouchPointTapped(touchIdx));
    // bool buttonDown = (input.mouse.leftDown && IsMouseWithinUiButton(button));
    bool buttonClicked = (input.mouse.leftPressed && IsMouseWithinUiButton(button));

    // Select a menu button
    if (input.menu.confirm || buttonClicked)
    {
        if (ui.currentMenu == UI_MENU_NONE && !game.paused)
            return; // not a menu

        if (button->onClick)
        {
            button->onClick();
            PlaySound(ui.sounds.menu);
        }
    }

    if (button->slider)
    {
        // Adjust slider with mouse/touch
        if (input.mouse.leftDown && ui.lastSelectWithMouse)
            UpdateUiSlider(button->slider);
        else
            button->slider->active = false;

        // Adjust slider with input actions
        static AutoRepeatSettings autoRepeat = { .triggerTime = 0.6f, .fireInterval = 0.1f };
        bool validInput = (input.menu.moveLeft || input.menu.moveRight);
        if (AutoRepeatShouldFire(&autoRepeat, validInput))
        {
            float newValue = button->slider->getValue();
            if (input.menu.moveLeft)
                newValue -= button->slider->increment;
            else if (input.menu.moveRight)
                newValue += button->slider->increment;
            button->slider->setValue(newValue, button->slider->min, button->slider->max);
        }
    }
}

void UpdateUiSlider(UiSlider *slider)
{
    slider->active = true;
    float newValue = Remap(input.mouse.uiPosition.x,
                           slider->rect.x, slider->rect.x + slider->rect.width,
                           slider->min, slider->max);
    slider->setValue(newValue, slider->min, slider->max);
}

// Touch screen virtual gamepad
// ----------------------------------------------------------------------------
void UpdateUiTouchInput(UiButton *button, UiInputTrigger onPressOrHold)
{
    int touchIdx = CheckCollisionTouchCircle(button->position, button->radius);
    bool isValidPress = (touchIdx != -1);
    if (onPressOrHold == UI_INPUT_ON_PRESS)
        isValidPress = IsTouchPointTapped(touchIdx);
    if (isValidPress)
        SetTouchPointButton(touchIdx, button->inputActionId);

    SetTouchInputActionDown(button->inputActionId, isValidPress);
    button->clicked = isValidPress;
}

void UpdateUiDPad(UiDPad *dpad)
{
    for (int i = 0; i < 4; i++)
    {
        int touchIdx = CheckCollisionTouchRec(dpad->button[i]);
        bool isValidPress = (touchIdx != -1);
        if (isValidPress)
            SetTouchPointButton(touchIdx, dpad->inputActionId[i]);

        SetTouchInputActionDown(dpad->inputActionId[i], isValidPress);
        dpad->clicked[i] = isValidPress;
    }
}

void UpdateUiAnalogStick(UiAnalogStick *stick)
{
    int touchIdx;
    if ((stick->lastTouchId != -1) &&
        (input.touchPoints[stick->lastTouchId].pressedCurrentFrame))
        touchIdx = stick->lastTouchId;
    else
        touchIdx = CheckCollisionTouchCircle(stick->centerPos, stick->centerRadius);

    // not touching analog stick
    if (touchIdx == -1 || input.touchCount == 0 || IsTouchingAnyButton(touchIdx))
    {
        stick->stickPos = stick->centerPos;
        stick->active = false;
        stick->lastTouchId = -1;
        return;
    }

    // is touching analog stick
    stick->lastTouchId = input.touchPoints[touchIdx].id;
    Vector2 touchPos = input.touchPoints[touchIdx].uiPosition;

    bool isTouchWithinStick =
        CheckCollisionPointCircle(touchPos, stick->centerPos, stick->centerRadius);
    if (isTouchWithinStick)
    {
        stick->stickPos = touchPos;
        stick->active = true;
    }
    else if (stick->active) // calculate position for edge of stick
    {
        Vector2 direction = Vector2Subtract(touchPos, stick->centerPos);
        float distance = Vector2Length(direction);
        if (distance > stick->centerRadius)
            direction = Vector2Scale(direction, stick->centerRadius / distance);
        stick->stickPos = Vector2Add(stick->centerPos, direction);
    }
}

void ChangeUiMenu(UiMenuState newMenu)
{
    ProcessUserInput(INPUT_POLL_MENU);
    ui.currentMenu = newMenu;
    ui.selectedId = 0;
    ui.firstFrame = true;
}

bool IsMouseWithinUiButton(UiButton *button)
{
    Vector2 mousePos = input.mouse.uiPosition;

    return CheckCollisionPointRec(mousePos, button->rect);
}

int IsTouchWithinUiButton(UiButton *button)
{
    return CheckCollisionTouchRec(button->rect);
}

// Draw
// ----------------------------------------------------------------------------

void DrawUiFrame(void)
{
    // Menus and buttons
    // ----------------------------------------------------------------------------
    if (ui.currentMenu != UI_MENU_NONE) // Draw non-gameplay menu
    {
        UiMenu *menu = &ui.menus[ui.currentMenu];
        for (int i = 0; i < arrlen(menu->buttons); i++)
            DrawUiButton(&menu->buttons[i]);

        for (int i = 0; i < arrlen(menu->text); i++)
        {
            UiText *textElem = &menu->text[i];
            DrawText(textElem->text, (int)textElem->position.x, (int)textElem->position.y,
                     textElem->fontSize, textElem->color);
        }
    }
    else if (game.currentScreen == SCREEN_GAMEPLAY && input.touchMode)
    {
        // Touch screen input controls
        DrawUiInputButton(&ui.gamepad.pause);
        DrawUiInputButton(&ui.gamepad.a);
        DrawUiInputButton(&ui.gamepad.x);
        if (ui.gamepad.dpad.enabled) DrawUiDPad(&ui.gamepad.dpad);
        if (ui.gamepad.stick.enabled) DrawUiAnalogStick(&ui.gamepad.stick);
    }

    // Gameplay UI
    // ----------------------------------------------------------------------------
    if (game.currentScreen == SCREEN_GAMEPLAY)
    {
    }

    // Debug info
    if (game.debugMode) DrawDebugInfo();
}

void DrawUiButton(UiButton *button)
{
    Color buttonColor = button->color;
    Color outlineColor = RAYWHITE;
    float buttonFontSize = (float)button->fontSize;
    Vector2 buttonPos = button->position;
    Vector2 checkboxPos = { 0 };
    if (button->checkbox)
        checkboxPos = button->checkbox->position;
    UiMenu *menu = &ui.menus[ui.currentMenu];
    bool isMouseHovering = ui.lastSelectWithMouse && (button == &menu->buttons[ui.mouseHoverId]);
    bool isSelectedButton = !ui.lastSelectWithMouse && (button == &menu->buttons[ui.selectedId]);
    UiSelectionStyle selectionStyleFlags = menu->selectStyleFlags;

    if (isMouseHovering || isSelectedButton)
    {
        if (selectionStyleFlags & UI_SELSTYLE_CURSOR)  DrawUiCursor(button);
        if (selectionStyleFlags & UI_SELSTYLE_HL_RECT) outlineColor = UI_SELECT_COLOR;
        if (selectionStyleFlags & UI_SELSTYLE_HL_TEXT) buttonColor = UI_SELECT_COLOR;
        if (selectionStyleFlags & UI_SELSTYLE_UNDERLINE)
            DrawLineEx((Vector2){ button->rect.x, button->rect.y + button->height },
                       (Vector2){ button->rect.x + button->width, button->rect.y + button->height },
                       8, buttonColor);
        if (selectionStyleFlags & UI_SELSTYLE_GROW)
        {
            buttonFontSize *= UI_SELECT_GROWTH_MULT;
            buttonPos = button->growPos;
            if (button->checkbox)
                checkboxPos = button->checkbox->growPos;
        }
    }

    if (button->bordered)
    {
        int outlineWidth = UI_BORDER_THICKNESS;
        Color boxColor = BLACK;
        if (button->clicked)
            boxColor = ColorAlpha(boxColor, UI_TRANSPARENCY*3);
        else
            boxColor = ColorAlpha(boxColor, UI_TRANSPARENCY);
        // box around text
        DrawRectangleRec(button->rect, boxColor);
        // outline box
        DrawRectangleLinesEx(button->rect, (float)outlineWidth, outlineColor);
    }

    if (button->checkbox)
    {
        // DrawRectangleRec(button->checkbox->rect, RAYWHITE);
        DrawCircleV(checkboxPos, button->checkbox->radius, DARKGRAY);
        if (button->checkbox->getValue())
            DrawCircleV(checkboxPos, button->checkbox->radius/2, buttonColor);
    }

    if (button->slider)
    {
        DrawRectangleRec(button->slider->rect, DARKGRAY);
        Rectangle sliderFilled = button->slider->rect;
        sliderFilled.width = button->slider->getValue()/button->slider->max*button->slider->rect.width;
        DrawRectangleRec(sliderFilled, buttonColor);
    }

    DrawText(button->text, (int)buttonPos.x, (int)buttonPos.y,
             (int)buttonFontSize, buttonColor);
}

void DrawUiCursor(UiButton *selectedButton)
{
    float size = UI_CURSOR_SIZE;

    Vector2 selectPointPos; // the corner/vertice pointing towards the right
    Vector2 cursorOffset = (Vector2){ -50.0f, (float)selectedButton->fontSize/2 };
    Vector2 buttonPos = (Vector2){ selectedButton->rect.x, selectedButton->rect.y };
    selectPointPos = Vector2Add(buttonPos, cursorOffset);

    DrawTriangle(Vector2Add(selectPointPos, (Vector2){ -size*2, size }),
                 selectPointPos,
                 Vector2Add(selectPointPos, (Vector2){ -size*2, -size }),
                 RAYWHITE);
}

void DrawUiInputButton(UiButton *button)
{
    // Color buttonColor = RAYWHITE;
    // if (button->clicked)
    //     buttonColor = ColorAlpha(buttonColor, UI_TRANSPARENCY*3);
    // else
    //     buttonColor = ColorAlpha(buttonColor, UI_TRANSPARENCY);
    // DrawCircleV(button->position, button->radius, buttonColor);
    DrawSpriteCircle(&button->texture, button->position, button->radius, 0, button->textureScale);
}

void DrawUiDPad(UiDPad *dpad)
{
    // for (int i = 0; i < 4; i++)
    // {
    //     Color buttonColor = RAYWHITE;
    //     if (dpad->clicked[i])
    //         buttonColor = ColorAlpha(buttonColor, UI_TRANSPARENCY*3);
    //     else
    //         buttonColor = ColorAlpha(buttonColor, UI_TRANSPARENCY);

    //     DrawRectangleRec(dpad->button[i], buttonColor);
    // }
    DrawSpriteRectangle(&dpad->texture, dpad->rect, 0);
}

void DrawUiAnalogStick(UiAnalogStick *stick)
{
    // DrawCircleV(stick->centerPos, stick->centerRadius, ColorAlpha(RAYWHITE, UI_TRANSPARENCY));
    DrawSpriteCircle(&stick->textureBase, stick->centerPos, stick->centerRadius, 0, 1.0f);
    // DrawRing(stick->centerPos, stick->centerRadius - 4, stick->centerRadius + 4,
    //          0, 360, 0, RAYWHITE);
    // DrawCircleV(stick->stickPos, stick->stickRadius, GRAY);
    DrawSpriteCircle(&stick->textureNub, stick->stickPos, stick->stickRadius, 0, 1.0f);
}

void DrawDebugInfo(void)
{
    const int textSize = 10;
    int textY = 0;
    DrawText(TextFormat("%i touchCount", input.touchCount), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    DrawText(TextFormat("mouse: %3.0f, %3.0f", input.mouse.uiPosition.x, input.mouse.uiPosition.y), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    if (input.touchCount > 0)
    {
        for (int i = 0; i < input.touchCount; i++)
        {
            DrawText(TextFormat("touch %i: %3.0f, %3.0f", i, GetTouchPosition(i).x, GetTouchPosition(i).y), 0, textY, textSize, RAYWHITE);
            textY += textSize;
        }
    }
}
