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
        .preventMouseClick = true,
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
    CreateUiMenuButton("Back", UiCallbackGoBack, 0, VIRTUAL_HEIGHT*0.35f);
    CreateUiMenuButtonRelative("Fullscreen:", UiCallbackToggleFullscreen);
    CreateUiCheckbox(UiCallbackCheckFullscreen);
    CreateUiMenuButtonRelative("Volume:", 0);
    CreateUiSlider(UiCallbackSetVolume, GetMasterVolume, 0.0f, 1.0f, 0.1f);
    CreateUiMenuButtonRelative("Render scale:", 0);
    CreateUiSlider(UiCallbackSetRenderScale, UiCallbackGetRenderScale, 1/4.0f, 4.0f, 1/4.0f);

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
    CreateUiMenuButton("Resume", UiCallbackResume, 0, VIRTUAL_HEIGHT*0.35f);
    CreateUiMenuButtonRelative("Settings", UiCallbackSettings);
    CreateUiMenuButtonRelative("Title Screen", UiCallbackGoToTitle);
    if (PLATFORM_CAN_EXIT)
        CreateUiMenuButtonRelative("Exit Game", UiCallbackExit);

    // Sound assets
    ui.sounds.menu =  LoadSoundAsset(&ui.assets, "assets/audio/menu_beep.wav");

    // Textures
    ui.textures.atlas       = LoadTextureAsset(&ui.assets, "assets/textures/controls.png");
    ui.textures.analogBase  = (Rectangle){   0.5f,   0, 256, 256 };
    ui.textures.analogStick = (Rectangle){   0.5f, 256, 128, 128 };
    ui.textures.pause       = (Rectangle){ 128.5f, 256, 128, 128 };
    ui.textures.dpad        = (Rectangle){ 256.5f,   0, 256, 256 };
    // ui.textures.a           = (Rectangle){ 256.5f, 256, 128, 128 };
    // ui.textures.b           = (Rectangle){ 384.5f, 256, 128, 128 };
    // ui.textures.x           = (Rectangle){   0.5f, 384, 128, 128 };
    // ui.textures.y           = (Rectangle){ 128.5f, 384, 128, 128 };

    // Touch input buttons (virtual gamepad)
    // ----------------------------------------------------------------------------
    // Analog stick
    ui.gamepad.stick = (UiAnalogStick){
        .sprite = &ui.textures.atlas,
        .spriteBaseRec = ui.textures.analogBase,
        .spriteStickRec = ui.textures.analogStick,
        .lastTouchId = -1,
        .enabled = false,
    };

    // D-Pad
    ui.gamepad.dpad = (UiDPad){
        .sprite = &ui.textures.atlas,
        .spriteRec = ui.textures.dpad,
        .inputActionId[UI_DPAD_UP] = INPUT_ACTION_UP,
        .inputActionId[UI_DPAD_DOWN] = INPUT_ACTION_DOWN,
        .inputActionId[UI_DPAD_LEFT] = INPUT_ACTION_LEFT,
        .inputActionId[UI_DPAD_RIGHT] = INPUT_ACTION_RIGHT,
        .enabled = true,
    };

    // Pause button
    ui.gamepad.pause = (UiButton){
        .sprite = &ui.textures.atlas,
        .spriteRec = ui.textures.pause,
        .text = "Pause",
        .spriteScale = 1.333f,
        .inputActionId = INPUT_ACTION_PAUSE,
        .color = RAYWHITE
    };

    UpdateUiGamepadRender();
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
        .rec = (Rectangle){
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
        button->rec.x = originButton->rec.x;
        button->position.x = button->rec.x + button->width/2 - textLength/2;
    }
    button->rec.y = originButton->rec.y + originButton->height + menu->spacing;
    button->position.y = originButton->position.y + originButton->height + menu->spacing;
    button->growPos.y = originButton->growPos.y + originButton->height + menu->spacing;
}

UiButton InitUiInputButton(char *text, int inputActionId, float x, float y, float radius)
{
    UiButton button = {
        .text = text,
        .spriteScale = 1.333f,
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
            button->rec.y + button->height/2,
        };
    newCheckbox.growPos =
        (Vector2){
            button->growPos.x + growLength + radius + 25,
            button->rec.y + button->height/2,
        };

    // reposition rect if necessary
    if (ui.menus[ui.initMenu].buttonWidth == 0)
    {
        button->rec.width += radius*2 + 25;
        button->rec.x -= radius*0.75f + 25;
    }

    button->checkbox = arena_alloc(&ui.arena, sizeof(UiCheckbox));
    *button->checkbox = newCheckbox;
}

void CreateUiSlider(UiSetFloatFunc setValue, UiGetFloatFunc getValue, float minValue, float maxValue, float increment)
{
    UiButton *button = ui.lastButtonCreated;

    UiSlider newSlider = {
        .rec = (Rectangle){
            button->rec.x + UI_BORDER_THICKNESS*3,
            button->rec.y + button->height,
            button->width - UI_BORDER_THICKNESS*6, button->height/2 - UI_BORDER_THICKNESS*3
        },
        .min = minValue, .max = maxValue,
        .setValue = setValue,
        .getValue = getValue,
        .increment = increment
    };

    // resize button
    button->rec.height += button->height/2;
    button->height += button->height/2;

    button->slider = arena_alloc(&ui.arena, sizeof(UiSlider));
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
    arena_free(&ui.arena);
    FreeRaylibAssets(&ui.assets);
}

// Update / User Input
// ----------------------------------------------------------------------------

void UpdateUiFrame(void)
{
    if (input.global.debug)
        game.isDebugMode = !game.isDebugMode;

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

    // Update timers
    if (ui.actionCooldownTimer > 0) ui.actionCooldownTimer -= game.frameTime;

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

    // Update touch gamepad for window size
    UpdateUiGamepadRender();

    // Update specific text fade
    if (game.isPaused)
    {
        UiText *pauseText = &ui.menus[UI_MENU_PAUSE].text[0];
        pauseText->color = Fade(RAYWHITE, ui.textFade);
        UiText *settingsText = &ui.menus[UI_MENU_SETTINGS].text[0];
        settingsText->color = Fade(RAYWHITE, ui.textFade);
    }
    // Update message timer when unpaused
    else if (ui.messageTimer > 0) ui.messageTimer -= game.frameTime;

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

    // Re-enable mouse click
    if ((ui.preventMouseClick && (input.touchCount == 0) &&
         !input.mouse.leftDown && !input.mouse.rightDown))
        ui.preventMouseClick = false;

    // Play sound when new item selected
    bool cursorMoved = (ui.selectedId != (int)prevId);

    if ((cursorMoved || newMouseHover) && !ui.firstFrame && !input.touchMode)
        PlaySound(ui.sounds.menu);

    ui.firstFrame = false;
}

void UpdateUiButtonSelect(UiButton *button)
{
    if (ui.selectedId == -1) return;

    // int touchIdx = IsTouchWithinUiButton(button);
    // bool buttonTapped = ((touchIdx != -1) && IsTouchPointTapped(touchIdx));
    // bool buttonDown = (input.mouse.leftDown && IsMouseWithinUiButton(button));
    bool buttonClicked = (input.mouse.leftPressed && IsMouseWithinUiButton(button));

    // Select a menu button
    if (input.menu.confirm || buttonClicked)
    {
        if (ui.currentMenu == UI_MENU_NONE && !game.isPaused)
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
        if (!ui.preventMouseClick && input.mouse.leftDown && ui.lastSelectWithMouse)
            UpdateUiSliderSelect(button->slider);
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
            button->slider->setValue(newValue, button->slider);
        }
    }
}

void UpdateUiSliderSelect(UiSlider *slider)
{
    if (input.mouse.leftPressed)
        slider->active = true;
    if (slider->active)
    {
        float newValue = Remap(input.mouse.uiPosition.x,
                               slider->rec.x, slider->rec.x + slider->rec.width,
                               slider->min, slider->max);
        slider->setValue(newValue, slider);
    }
}

// Touch screen virtual gamepad
// ----------------------------------------------------------------------------
void UpdateUiGamepadRender(void)
{
    int winWidth = GetRenderWidth();
    int winHeight = GetRenderHeight();
    float scale = ui.camera.zoom;
    float padding = UI_INPUT_PADDING*scale;

    UiAnalogStick *stick = &ui.gamepad.stick;
    stick->centerRadius = UI_STICK_RADIUS*scale;
    stick->stickRadius = UI_STICK_RADIUS/2*scale;
    stick->centerPos.x = stick->centerRadius + padding;
    stick->centerPos.y = winHeight - stick->centerRadius - padding;
    stick->stickPos = stick->centerPos;

    UiDPad *dpad = &ui.gamepad.dpad;
    dpad->width = UI_DPAD_WIDTH*scale;
    dpad->button[UI_DPAD_UP].width = dpad->width*3;
    dpad->button[UI_DPAD_UP].height = dpad->width;
    dpad->button[UI_DPAD_DOWN].width = dpad->width*3;
    dpad->button[UI_DPAD_DOWN].height = dpad->width;
    dpad->button[UI_DPAD_LEFT].width = dpad->width;
    dpad->button[UI_DPAD_LEFT].height = dpad->width*3;
    dpad->button[UI_DPAD_RIGHT].width = dpad->width;
    dpad->button[UI_DPAD_RIGHT].height = dpad->width*3;
    dpad->rec.width = dpad->width*3;
    dpad->rec.height = dpad->width*3;
    dpad->position.x = dpad->width*1.5f + padding;
    dpad->position.y = winHeight - dpad->width*1.5f - padding;
    dpad->button[UI_DPAD_UP] =
        (Rectangle){ dpad->position.x - dpad->width*1.5f,
            dpad->position.y - dpad->width*1.5f,
            dpad->width*3, dpad->width };
    dpad->button[UI_DPAD_DOWN] =
        (Rectangle){ dpad->position.x - dpad->width*1.5f,
            dpad->position.y + dpad->width*0.5f,
            dpad->width*3, dpad->width };
    dpad->button[UI_DPAD_LEFT] =
        (Rectangle){ dpad->position.x - dpad->width*1.5f,
            dpad->position.y - dpad->width*1.5f,
            dpad->width, dpad->width*3 };
    dpad->button[UI_DPAD_RIGHT] =
        (Rectangle){ dpad->position.x + dpad->width*0.5f,
            dpad->position.y - dpad->width*1.5f,
            dpad->width, dpad->width*3 };
    dpad->rec =
        (Rectangle){ dpad->button[UI_DPAD_UP].x, dpad->button[UI_DPAD_UP].y,
            dpad->width*3, dpad->width*3 };

    UiButton *pause = &ui.gamepad.pause;
    pause->radius = UI_INPUT_RADIUS*0.8f*scale;
    pause->position.x = winWidth - pause->radius - padding;
    pause->position.y = winHeight - dpad->width - padding;
}

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
    Vector2 touchPos = input.touchPoints[touchIdx].position;

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
    ui.preventMouseClick = true;
}

bool IsMouseWithinUiButton(UiButton *button)
{
    Vector2 mousePos = input.mouse.uiPosition;

    return CheckCollisionPointRec(mousePos, button->rec);
}

int IsTouchWithinUiButton(UiButton *button)
{
    return CheckCollisionTouchRec(button->rec);
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

    // Gameplay UI
    // ----------------------------------------------------------------------------
    if (game.currentScreen == SCREEN_GAMEPLAY)
    {
        if (!game.isPaused && ui.messageTimer > 0)
            DrawText(ui.timedMessage.text, (int)ui.timedMessage.position.x, (int)ui.timedMessage.position.y,
                     ui.timedMessage.fontSize, ui.timedMessage.color);
    }

    // Debug info
    if (game.isDebugMode) DrawDebugInfo();
}

void DrawUiGamepad(void)
{
    if (input.touchMode &&
        ui.currentMenu == UI_MENU_NONE && game.currentScreen == SCREEN_GAMEPLAY)
    {
        // Touch screen input controls
        DrawUiInputButton(&ui.gamepad.pause);
        if (ui.gamepad.dpad.enabled) DrawUiDPad(&ui.gamepad.dpad);
        if (ui.gamepad.stick.enabled) DrawUiAnalogStick(&ui.gamepad.stick);
    }
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
            DrawLineEx((Vector2){ button->rec.x, button->rec.y + button->height },
                       (Vector2){ button->rec.x + button->width, button->rec.y + button->height },
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
        DrawRectangleRec(button->rec, boxColor);
        // outline box
        DrawRectangleLinesEx(button->rec, (float)outlineWidth, outlineColor);
    }

    if (button->checkbox)
    {
        // DrawRectangleRec(button->checkbox->rec, RAYWHITE);
        DrawCircleV(checkboxPos, button->checkbox->radius, DARKGRAY);
        if (button->checkbox->getValue())
            DrawCircleV(checkboxPos, button->checkbox->radius/2, buttonColor);
    }

    if (button->slider)
    {
        DrawRectangleRec(button->slider->rec, DARKGRAY);
        Rectangle sliderFilled = button->slider->rec;
        sliderFilled.width = (button->slider->getValue()/button->slider->max)*button->slider->rec.width;
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
    Vector2 buttonPos = (Vector2){ selectedButton->rec.x, selectedButton->rec.y };
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
    DrawSpriteOnCircle(button->sprite, button->spriteRec, button->position, button->radius, 0);
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
    DrawSpriteOnRectangle(dpad->sprite, dpad->spriteRec, dpad->rec, 0);
    // DrawTexturePro(ui.textures.atlas, dpad->spriteRec, dpad->rec, Vector2Zero(), 0, WHITE);
}

void DrawUiAnalogStick(UiAnalogStick *stick)
{
    // DrawCircleV(stick->centerPos, stick->centerRadius, ColorAlpha(RAYWHITE, UI_TRANSPARENCY));
    DrawSpriteOnCircle(stick->sprite, stick->spriteBaseRec, stick->centerPos, stick->centerRadius, 0);
    // DrawRing(stick->centerPos, stick->centerRadius - 4, stick->centerRadius + 4,
    //          0, 360, 0, RAYWHITE);
    // DrawCircleV(stick->stickPos, stick->stickRadius, GRAY);
    DrawSpriteOnCircle(stick->sprite, stick->spriteStickRec, stick->stickPos, stick->stickRadius, 0);
}

void DrawDebugInfo(void)
{
    DrawFPS(0, 0);
    const int textSize = 20;
    int textY = 20;
    DrawText(TextFormat("%i touchCount", input.touchCount), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    DrawText(TextFormat("%i leftDown, %i rightDown", input.mouse.leftDown, input.mouse.rightDown), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    DrawText(TextFormat("mouse: %3.0f, %3.0f", input.mouse.uiPosition.x, input.mouse.uiPosition.y), 0, textY, textSize, RAYWHITE);
    textY += textSize;
    DrawText(TextFormat("render res: %.0f, %.0f", viewport.renderTexWidth, viewport.renderTexHeight), 0, textY, textSize, RAYWHITE);
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

// Other
// ----------------------------------------------------------------------------
void SetTimedMessage(char *message, int fontSize, float time, Color color)
{
    ui.messageTimer = time;
    int messageLength = MeasureText(message, fontSize);

    ui.timedMessage = (UiText){ .text = message,
        .position = { (float)(VIRTUAL_WIDTH - messageLength)/2, (float)(VIRTUAL_HEIGHT - fontSize)/2 },
        .fontSize = fontSize,
        .color = color
    };
}
