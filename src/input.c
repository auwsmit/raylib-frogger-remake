// EXPLANATION:
// Helps manage and handle game input
// See header for more documentation/descriptions

InputActionMaps inputMaps; // contains defined input action controls

void InitDefaultInputSettings(void)
{
    // Setup default input
    input = (InputState){
        // Analog sticks deadzone input
        .gamepad.leftStickDeadzone = 0.25f,
        .gamepad.rightStickDeadzone = 0.25f,
        .gamepad.leftTriggerDeadzone = -0.9f,
        .gamepad.rightTriggerDeadzone = -0.9f,
    };

    // For touch point UI button tracking
    for (int i = 0; i < INPUT_MAX_TOUCH_POINTS; i++)
        input.touchPoints[i].currentButton = -1;

    // Setup input action control mappings
    inputMaps = (InputActionMaps){
        // Global controls
        .gamepadButton[INPUT_ACTION_FULLSCREEN] = { GAMEPAD_BUTTON_SELECT },
        .key[INPUT_ACTION_FULLSCREEN] = {
            KEY_LEFT_ALT, KEY_ENTER,  // alt+enter
            KEY_RIGHT_ALT, KEY_ENTER, // ^ ^ ^ ^ ^
            KEY_LEFT_SHIFT, KEY_F,  // shift+f
            KEY_RIGHT_SHIFT, KEY_F, // ^ ^ ^ ^
            KEY_F11,
        },
        .key[INPUT_ACTION_DEBUG] = { KEY_F3 },

        // Menu controls
        .gamepadButton[INPUT_ACTION_CONFIRM]    = { GAMEPAD_BUTTON_SOUTH },
        .gamepadButton[INPUT_ACTION_CANCEL]     = { GAMEPAD_BUTTON_EAST },
        .gamepadButton[INPUT_ACTION_MENU_UP]    = { GAMEPAD_DPAD_UP },
        .gamepadButton[INPUT_ACTION_MENU_DOWN]  = { GAMEPAD_DPAD_DOWN },
        .gamepadButton[INPUT_ACTION_MENU_LEFT]  = { GAMEPAD_DPAD_LEFT },
        .gamepadButton[INPUT_ACTION_MENU_RIGHT] = { GAMEPAD_DPAD_RIGHT },
        .gamepadAxis[INPUT_ACTION_MENU_UP]      = { GAMEPAD_AXIS_LEFT_Y, -INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_MENU_DOWN]    = { GAMEPAD_AXIS_LEFT_Y, INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_MENU_LEFT]    = { GAMEPAD_AXIS_LEFT_X, -INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_MENU_RIGHT]   = { GAMEPAD_AXIS_LEFT_X, INPUT_ANALOG_MENU_DEADZONE },
        .key[INPUT_ACTION_CONFIRM]              = { KEY_ENTER, KEY_SPACE },
        .key[INPUT_ACTION_CANCEL]               = { KEY_ESCAPE, KEY_BACKSPACE, },
        .key[INPUT_ACTION_MENU_UP]              = { KEY_W, KEY_UP },
        .key[INPUT_ACTION_MENU_DOWN]            = { KEY_S, KEY_DOWN },
        .key[INPUT_ACTION_MENU_LEFT]            = { KEY_A, KEY_LEFT },
        .key[INPUT_ACTION_MENU_RIGHT]           = { KEY_D, KEY_RIGHT },

        // Player 1 controls
        .gamepadButton[INPUT_ACTION_PAUSE] = { GAMEPAD_BUTTON_START },
        .gamepadButton[INPUT_ACTION_UP]    = { GAMEPAD_DPAD_UP },
        .gamepadButton[INPUT_ACTION_DOWN]  = { GAMEPAD_DPAD_DOWN },
        .gamepadButton[INPUT_ACTION_LEFT]  = { GAMEPAD_DPAD_LEFT },
        .gamepadButton[INPUT_ACTION_RIGHT] = { GAMEPAD_DPAD_RIGHT },
        .gamepadAxis[INPUT_ACTION_UP]      = { GAMEPAD_AXIS_LEFT_Y, -INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_DOWN]    = { GAMEPAD_AXIS_LEFT_Y, INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_LEFT]    = { INPUT_GAMEPAD_AXIS_LEFT_X, -INPUT_ANALOG_MENU_DEADZONE },
        .gamepadAxis[INPUT_ACTION_RIGHT]   = { INPUT_GAMEPAD_AXIS_LEFT_X, INPUT_ANALOG_MENU_DEADZONE },
        .key[INPUT_ACTION_PAUSE]           = { KEY_P, KEY_ESCAPE },
        .key[INPUT_ACTION_UP]              = { KEY_W, KEY_UP, },
        .key[INPUT_ACTION_DOWN]            = { KEY_S, KEY_DOWN, },
        .key[INPUT_ACTION_LEFT]            = { KEY_A, KEY_LEFT, },
        .key[INPUT_ACTION_RIGHT]           = { KEY_D, KEY_RIGHT, },
    };
    if (!PLATFORM_HAS_ANALOG_TRIGGERS)
    {
        // AddInputActionGamepadButton(INPUT_ACTION_THRUST, GAMEPAD_BUTTON_L2);
        // AddInputActionGamepadButton(INPUT_ACTION_SHOOT, GAMEPAD_BUTTON_R2);
    }
}

void AddInputActionGamepadButton(InputAction action, GamepadButton button)
{
    for (int i = 0; i < INPUT_MAX_MAPS; i++)
    {
        if (inputMaps.gamepadButton[action][i] == 0)
        {
            inputMaps.gamepadButton[action][i] = button;
            break;
        }
    }
}

void UpdateInputFrame(void)
{
    ProcessUserInput(INPUT_POLL_ALL);
    if (input.cancelTime > 0)
    {
        input.cancelTime -= game.frameTime;
        input.mouse.moved = false;
        CancelInputActions();
    }
}

void ProcessUserInput(InputPollFlag pollType)
{
    if (pollType & INPUT_POLL_KEYBOARD)
    {
        KeyboardKey currentKey = GetKeyPressed();
        if (IsInputKeyModifier(currentKey))
            input.anyKeyPressed = false;
        else
            input.anyKeyPressed = (currentKey != 0);
    }

    if (pollType & INPUT_POLL_MOUSE)
    {
        Vector2 mousePos = GetMousePosition();
        // adjust for window offset and scale (not needed for UI)
        Vector2 gameMousePos = {
            (mousePos.x - render.x)/render.scale,
            (mousePos.y - render.y)/render.scale
        };
        input.mouse.gamePosition = GetScreenToWorld2D(gameMousePos, game.camera);
        input.mouse.uiPosition   = GetScreenToWorld2D(mousePos, ui.camera);
        input.mouse.delta        = Vector2Scale(GetMouseDelta(), 1.0f/game.camera.zoom);
        input.mouse.moved        = (Vector2Length(input.mouse.delta) > 0);
        input.mouse.leftPressed  = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        input.mouse.leftDown     = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        input.mouse.rightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        input.mouse.rightDown    = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        input.mouse.pressed      = (input.mouse.leftPressed || input.mouse.rightPressed);
    }

    // Detect and update gamepad
    if (pollType & INPUT_POLL_GAMEPAD)
    {
        input.gamepad.available = IsGamepadAvailable(input.gamepadId);
        if (input.gamepad.available)
        {
            input.gamepadButtonPressed    = GetGamepadButtonPressed();
            input.anyGamepadButtonPressed = IsGamepadButtonPressed(input.gamepadId, input.gamepadButtonPressed);
            input.gamepad.leftStickX      = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_LEFT_X);
            input.gamepad.leftStickY      = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_LEFT_Y);
            input.gamepad.rightStickX     = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_RIGHT_X);
            input.gamepad.rightStickY     = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_RIGHT_Y);
            if (PLATFORM_HAS_ANALOG_TRIGGERS)
            {
                input.gamepad.leftTrigger     = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_LEFT_TRIGGER);
                input.gamepad.rightTrigger    = GetGamepadAxisMovement(input.gamepadId, GAMEPAD_AXIS_RIGHT_TRIGGER);
            }

            for (int i = 0; i < INPUT_MAX_ACTIONS; i++)
            {
                if (inputMaps.gamepadAxis[i].axis == 0)
                    continue;
                input.gamepadAxisPressedPreviousFrame[i] = input.gamepadAxisPressedCurrentFrame[i];
                input.gamepadAxisPressedCurrentFrame[i] = IsInputActionAxisDown(i);
            }
        }
        else input.anyGamepadButtonPressed = false;
    }

    input.anyInputPressed = (input.mouse.pressed || input.anyGamepadButtonPressed || input.anyKeyPressed);

    if (pollType & INPUT_POLL_TOUCH)
    {
        // Detect touch mode
        int tCount = GetTouchPointCount();
        input.touchCount = tCount;

        if (input.touchCount == 0)
        {
            if (input.mouse.leftDown || input.mouse.rightDown || input.anyKeyPressed)
                input.touchMode = false;
        }
        else input.touchMode = true;

        if (input.touchMode)
        {
            // Update touch points
            if (tCount > INPUT_MAX_TOUCH_POINTS)
                tCount = INPUT_MAX_TOUCH_POINTS;
            for (int i = 0; i < tCount; i++)
            {
                Vector2 touchPos = GetTouchPosition(i);
                Vector2 gameTouchPos = {
                    (touchPos.x - render.x)/render.scale,
                    (touchPos.y - render.y)/render.scale
                };
                TouchPoint *touchPoint           = &input.touchPoints[i];
                touchPoint->gamePosition         = GetScreenToWorld2D(gameTouchPos, game.camera);
                touchPoint->position             = touchPos;
                touchPoint->pressedPreviousFrame = touchPoint->pressedCurrentFrame;
                touchPoint->pressedCurrentFrame  = true;
                touchPoint->currentButton        = -1;
                touchPoint->id                   = GetTouchPointId(i);
            }
            for (int i = tCount; i < INPUT_MAX_TOUCH_POINTS; i++)
                input.touchPoints[i].pressedCurrentFrame = false;


            // Process touch gamepad
            if (game.isPaused)
                SetTouchInputActionDown(INPUT_ACTION_PAUSE, false);
            else UpdateUiTouchInput(&ui.gamepad.pause, UI_INPUT_ON_PRESS);
            if (ui.gamepad.dpad.enabled) UpdateUiDPad(&ui.gamepad.dpad);
            if (ui.gamepad.stick.enabled) UpdateUiAnalogStick(&ui.gamepad.stick);
        }
    }

    // Check input mappings

    if (pollType & INPUT_POLL_GLOBAL)
    {
        input.global.fullscreen = IsInputActionPressed(INPUT_ACTION_FULLSCREEN);
        input.global.debug      = IsInputActionPressed(INPUT_ACTION_DEBUG);
    }

    if ((pollType & INPUT_POLL_MENU) || (ui.currentMenu != UI_MENU_NONE))
    {
        input.menu.confirm   = IsInputActionPressed(INPUT_ACTION_CONFIRM);
        input.menu.cancel    = IsInputActionPressed(INPUT_ACTION_CANCEL);
        input.menu.moveUp    = IsInputActionDown(INPUT_ACTION_MENU_UP);
        input.menu.moveDown  = IsInputActionDown(INPUT_ACTION_MENU_DOWN);
        input.menu.moveLeft  = IsInputActionDown(INPUT_ACTION_MENU_LEFT);
        input.menu.moveRight = IsInputActionDown(INPUT_ACTION_MENU_RIGHT);
    }

    if ((pollType & INPUT_POLL_PLAYER) || (game.currentScreen == SCREEN_GAMEPLAY))
    {
        input.player.pause     = IsInputActionPressed(INPUT_ACTION_PAUSE);
        input.player.moveUp    = IsInputActionPressed(INPUT_ACTION_UP);
        input.player.moveDown  = IsInputActionPressed(INPUT_ACTION_DOWN);
        input.player.moveLeft  = IsInputActionPressed(INPUT_ACTION_LEFT);
        input.player.moveRight = IsInputActionPressed(INPUT_ACTION_RIGHT);
    }

    if (input.mouseCancelled || input.mouse.moved)
        input.mouseCancelled = false;
    else if (input.mouseCancelled)
        CancelMouseInput();

    if (input.cancelled && (input.anyKeyPressed || input.anyGamepadButtonPressed))
        input.cancelled = false;
    else if (input.cancelled)
        CancelInputActions();
}

void CancelMouseInput(void)
{
    input.mouse  = (InputMouseState){ 0 };
    input.mouseCancelled = true;
}

void CancelInputActions(void)
{
    input.global  = (InputActionsGlobal){ 0 };
    input.menu    = (InputActionsMenu){ 0 };
    input.player  = (InputActionsPlayer){ 0 };
    input.gamepad = (InputGamepadState){ 0 };
    input.anyKeyPressed           = false;
    input.anyGamepadButtonPressed = false;
    input.anyInputPressed         = false;
    input.cancelled = true;
}
// Input Actions
// ----------------------------------------------------------------------------
bool IsInputKeyModifier(KeyboardKey key)
{
    if (key == KEY_LEFT_ALT     || key == KEY_RIGHT_ALT ||
        key == KEY_LEFT_SHIFT   || key == KEY_RIGHT_SHIFT ||
        key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
        return true;

    return false;
}

bool IsInputActionDown(InputAction action)
{
    // Check touch screen button
    if (input.touchButtonDown[action] == true)
        return true;

    // Check controller buttons
    if (input.gamepad.available)
    {
        GamepadButton *buttons = inputMaps.gamepadButton[action];
        for (int i = 0; i < INPUT_MAX_MAPS && buttons[i] != 0; i++)
        {
            GamepadButton button = buttons[i];
            if (IsGamepadButtonDown(input.gamepadId, button))
                return true;
        }
        if (IsInputActionAxisDown(action))
            return true;
    }

    // Check potential key combinations
    KeyboardKey* keys = inputMaps.key[action];
    for (int i = 0; i < INPUT_MAX_MAPS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        // Check modifier plus next key (only 1 modifier for now)
        if (IsInputKeyModifier(key))
        {
            if ((i + 1 < INPUT_MAX_MAPS) && (keys[i + 1] != 0) &&
                (!IsInputKeyModifier(keys[i + 1])))
            {
                if (IsKeyDown(key) && IsKeyDown(keys[i + 1]))
                    return true;
                i++;
            }

            // Check just the modifier by itself
            else if (IsKeyDown(key))
                return true;
        }

        else if (IsKeyDown(key))
            return true;
    }

    // Check mouse buttons
    if (IsInputActionMouseDown(action))
        return true;

    return false;
}

bool IsInputActionAxisDown(InputAction action)
{
    // Sticks and triggers
    GamepadAxis axis = inputMaps.gamepadAxis[action].axis;
    float deadzone = inputMaps.gamepadAxis[action].deadzone;
    float axisValue = 0.0f;
    if (axis == INPUT_GAMEPAD_AXIS_LEFT_X)
        axis = GAMEPAD_AXIS_LEFT_X;
    switch (axis)
    {
        case GAMEPAD_AXIS_LEFT_X: axisValue = input.gamepad.leftStickX;
                                  break;
        case GAMEPAD_AXIS_LEFT_Y: axisValue = input.gamepad.leftStickY;
                                  break;
        case GAMEPAD_AXIS_RIGHT_X: axisValue = input.gamepad.rightStickX;
                                   break;
        case GAMEPAD_AXIS_RIGHT_Y: axisValue = input.gamepad.rightStickY;
                                   break;
        case GAMEPAD_AXIS_LEFT_TRIGGER: axisValue = input.gamepad.leftTrigger;
                                        break;
        case GAMEPAD_AXIS_RIGHT_TRIGGER: axisValue = input.gamepad.rightTrigger;
                                         break;
        default: break;
    }

    bool negativeTrigger = ((deadzone < 0) && (axisValue < deadzone));
    bool positiveTrigger = ((deadzone > 0) && (axisValue > deadzone));
    if (negativeTrigger || positiveTrigger)
        return true;

    return false;
}

bool IsInputActionMouseDown(InputAction action)
{
    // if (input.touchMode)
    //     return false;

    MouseButton* mb = inputMaps.mouse[action];
    for (int i = 0; i < INPUT_MAX_MAPS && mb[i] != 0; i++)
    {
        MouseButton button = mb[i];
        if (button == INPUT_MOUSE_LEFT_BUTTON)
            button = MOUSE_LEFT_BUTTON;
        if (IsMouseButtonDown(button))
            return true;
    }

    return false;
}

bool IsInputActionPressed(InputAction action)
{
    // Check touch screen button
    if (input.touchButtonPressed[action] == true)
        return true;

    // Check controller input
    if (input.gamepad.available)
    {
        // buttons
        GamepadButton *buttons = inputMaps.gamepadButton[action];
        for (int i = 0; i < INPUT_MAX_MAPS && buttons[i] != 0; i++)
        {
            GamepadButton button = buttons[i];
            if (IsGamepadButtonPressed(input.gamepadId, button))
                return true;
        }
        if (IsInputActionAxisPressed(action))
            return true;
    }

    // Check potential key combinations
    KeyboardKey* keys = inputMaps.key[action];
    for (int i = 0; i < INPUT_MAX_MAPS && keys[i] != 0; i++)
    {
        KeyboardKey key = keys[i];

        // Check modifier plus next key (only 1 modifier for now)
        if (IsInputKeyModifier(key))
        {
            if ((i + 1 < INPUT_MAX_MAPS) && (keys[i + 1] != 0) &&
                (!IsInputKeyModifier(keys[i + 1])))
            {
                if (IsKeyDown(key) && IsKeyPressed(keys[i + 1]))
                    return true;
                i++; // Skip the next key
            }
            // Check just the modifier by itself
            else if (IsKeyPressed(key))
                return true;
        }

        // Check a single key
        else if (IsKeyPressed(key))
            return true;
    }

    // Check mouse buttons
    if (IsInputActionMousePressed(action))
        return true;

    return false;
}

bool IsInputActionAxisPressed(InputAction action)
{
    if (!input.gamepadAxisPressedPreviousFrame[action] &&
        input.gamepadAxisPressedCurrentFrame[action])
        return true;

    return false;
}

bool IsInputActionMousePressed(InputAction action)
{
    MouseButton* mb = inputMaps.mouse[action];
    for (int i = 0; i < INPUT_MAX_MAPS && mb[i] != 0; i++)
    {
        MouseButton button = mb[i];
        if (button == INPUT_MOUSE_LEFT_BUTTON)
            button = MOUSE_LEFT_BUTTON;
        if (IsMouseButtonPressed(button))
            return true;
    }

    return false;
}

// Touch / Virtual Input
// ----------------------------------------------------------------------------
void SetTouchInputActionDown(InputAction action, bool buttonDown)
{
    // prevent two buttons with the same action from interfering
    if (input.touchButtonFrameActive[action] == game.frameCount)
        return;
    else if (buttonDown)
        input.touchButtonFrameActive[action] = game.frameCount;

    // check and set if touch button was pressed (not held)
    if (buttonDown && !input.touchButtonDown[action])
        input.touchButtonPressed[action] = true;
    else
        input.touchButtonPressed[action] = false;

    input.touchButtonDown[action] = buttonDown;
}

void SetTouchPointButton(int index, int buttonIdx)
{
    input.touchPoints[index].currentButton = buttonIdx;
}

bool IsTouchPointTapped(int index)
{
    return (input.touchPoints[index].pressedCurrentFrame &&
            !input.touchPoints[index].pressedPreviousFrame);
}

bool IsTouchPointReleased(int index)
{
    return (input.touchPoints[index].pressedPreviousFrame &&
            !input.touchPoints[index].pressedCurrentFrame);
}

// bool IsTouchingButton(int index, int buttonId)
// {
//     return input.touchPoints[index].currentButton == buttonId;
// }

bool IsTouchingAnyButton(int index)
{
    return input.touchPoints[index].currentButton != -1;
}

int CheckCollisionTouchCircle(Vector2 center, float radius)
{
    for (int i = 0; i < input.touchCount; ++i)
        if (CheckCollisionPointCircle(input.touchPoints[i].position, center, radius))
            return i;

    return -1;
}

int CheckCollisionTouchRec(Rectangle rec)
{
    for (int i = 0; i < input.touchCount; ++i)
        if (CheckCollisionPointRec(input.touchPoints[i].position, rec))
            return i;

    return -1;
}

// Other
// ----------------------------------------------------------------------------
bool AutoRepeatShouldFire(AutoRepeatSettings *ar, bool fireCondition)
{
    bool shouldFire = false;
    bool initialPress = (!ar->active && ar->heldTime == 0);
    bool heldLongEnoughToRepeat = (ar->active && ar->heldTime >= 0.1f);

    if (fireCondition && (initialPress || heldLongEnoughToRepeat))
    {
        ar->heldTime = 0;
        shouldFire = true;
    }

    // Update auto-scroll timer
    if (fireCondition)
    {
        ar->heldTime += game.frameTime;
        if (ar->heldTime >= ar->triggerTime)
            ar->active = true;
    }
    else
    {
        ar->heldTime = 0;
        ar->active = false;
    }

    return shouldFire;
}
