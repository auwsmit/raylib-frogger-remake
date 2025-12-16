// EXPLANATION:
// Helps manage and handle game input

#ifndef FROGGER_INPUT_HEADER_GUARD
#define FROGGER_INPUT_HEADER_GUARD

// Macros
// ----------------------------------------------------------------------------
#define INPUT_MAX_ACTIONS 16 // Maximum number of game actions, e.g. confirm, pause, move up
#define INPUT_MAX_MAPS 24 // Maximum number of inputs that can be mapped to an action
#define INPUT_MAX_TOUCH_POINTS 8
#define INPUT_ANALOG_MENU_DEADZONE 0.5f // Deadzone used for analog stick menu movement
#define INPUT_ANALOG_GAME_DEADZONE 0.5f // Deadzone used for analog stick game movement
#define INPUT_TRIGGER_BUTTON_DEADZONE 0.25f // Deadzone used when trigger is used as a button

// These are needed because MOUSE_LEFT_BUTTON is 0, which is the default non-mapped value
#define INPUT_MOUSE_LEFT_BUTTON 7
// Same as above, but for GAMEPAD_AXIS_LEFT_X
#define INPUT_GAMEPAD_AXIS_LEFT_X 6

// Aliases (just a personal preference)
#define GAMEPAD_BUTTON_NORTH  GAMEPAD_BUTTON_RIGHT_FACE_UP
#define GAMEPAD_BUTTON_SOUTH  GAMEPAD_BUTTON_RIGHT_FACE_DOWN
#define GAMEPAD_BUTTON_EAST   GAMEPAD_BUTTON_RIGHT_FACE_RIGHT
#define GAMEPAD_BUTTON_WEST   GAMEPAD_BUTTON_RIGHT_FACE_LEFT
#define GAMEPAD_BUTTON_START  GAMEPAD_BUTTON_MIDDLE_RIGHT
#define GAMEPAD_BUTTON_SELECT GAMEPAD_BUTTON_MIDDLE_LEFT
#define GAMEPAD_BUTTON_HOME   GAMEPAD_BUTTON_MIDDLE
#define GAMEPAD_BUTTON_L1     GAMEPAD_BUTTON_LEFT_TRIGGER_1
#define GAMEPAD_BUTTON_L2     GAMEPAD_BUTTON_LEFT_TRIGGER_2
#define GAMEPAD_BUTTON_R1     GAMEPAD_BUTTON_RIGHT_TRIGGER_1
#define GAMEPAD_BUTTON_R2     GAMEPAD_BUTTON_RIGHT_TRIGGER_2
#define GAMEPAD_DPAD_UP       GAMEPAD_BUTTON_LEFT_FACE_UP
#define GAMEPAD_DPAD_DOWN     GAMEPAD_BUTTON_LEFT_FACE_DOWN
#define GAMEPAD_DPAD_LEFT     GAMEPAD_BUTTON_LEFT_FACE_LEFT
#define GAMEPAD_DPAD_RIGHT    GAMEPAD_BUTTON_LEFT_FACE_RIGHT

// Types and Structures
// ----------------------------------------------------------------------------
typedef enum {
    INPUT_ACTION_NULL,

    // global
    INPUT_ACTION_FULLSCREEN,
    INPUT_ACTION_DEBUG,

    // menu
    INPUT_ACTION_CONFIRM,
    INPUT_ACTION_CANCEL,
    INPUT_ACTION_MENU_UP,
    INPUT_ACTION_MENU_DOWN,
    INPUT_ACTION_MENU_LEFT,
    INPUT_ACTION_MENU_RIGHT,

    // player
    INPUT_ACTION_PAUSE,
    INPUT_ACTION_UP,
    INPUT_ACTION_DOWN,
    INPUT_ACTION_LEFT,
    INPUT_ACTION_RIGHT,
} InputAction;

typedef enum {
    INPUT_POLL_ALL      = (255 << 0),
    INPUT_POLL_MOUSE    = (1 << 1),
    INPUT_POLL_KEYBOARD = (1 << 2),
    INPUT_POLL_GAMEPAD  = (1 << 3),
    INPUT_POLL_TOUCH    = (1 << 4),
    INPUT_POLL_GLOBAL   = (1 << 5),
    INPUT_POLL_MENU     = (1 << 6),
    INPUT_POLL_PLAYER   = (1 << 7)
} InputPollFlag;

typedef struct {
    bool fullscreen;
    bool debug;
} InputActionsGlobal;

typedef struct {
    bool confirm;
    bool cancel;
    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;
} InputActionsMenu;

typedef struct {
    bool pause;
    bool moveUp;
    bool moveDown;
    bool moveLeft;
    bool moveRight;
} InputActionsPlayer;

typedef struct {
    Vector2 gamePosition;
    Vector2 uiPosition;
    Vector2 delta;
    bool moved;
    bool leftPressed;
    bool leftDown;
    bool rightPressed;
    bool rightDown;
    bool pressed;
} InputMouseState;

typedef struct {
    float leftStickDeadzone;
    float rightStickDeadzone;
    float leftTriggerDeadzone;
    float rightTriggerDeadzone;
    float leftStickX;
    float leftStickY;
    float rightStickX;
    float rightStickY;
    float leftTrigger;
    float rightTrigger;
    bool available;
} InputGamepadState;

typedef struct {
    Vector2 gamePosition;
    Vector2 uiPosition;
    bool isActive;
    bool pressedPreviousFrame;
    bool pressedCurrentFrame;
    int id;
    int currentButton; // TODO this should probably just be a bool
} TouchPoint;

typedef struct {
    GamepadAxis axis;
    float deadzone; // the threshold for axis to be 'pressed down' as a button
                    // used to map analog stick or trigger as buttons
} GamepadAxisMap;

typedef struct {
    KeyboardKey key[INPUT_MAX_ACTIONS][INPUT_MAX_MAPS];
    MouseButton mouse[INPUT_MAX_ACTIONS][4];
    GamepadButton gamepadButton[INPUT_MAX_ACTIONS][INPUT_MAX_MAPS];
    GamepadAxisMap gamepadAxis[INPUT_MAX_ACTIONS];
} InputActionMaps;

// Tracks input data for the current frame
typedef struct {
    // bools for input actions
    // e.g. if (input.global.fullscreen) ToggleFullscreen();
    InputActionsGlobal global;
    InputActionsMenu menu;
    InputActionsPlayer player;

    InputMouseState mouse;

    InputGamepadState gamepad;
    int gamepadId;
    int gamepadButtonPressed;
    bool gamepadAxisPressedCurrentFrame[INPUT_MAX_ACTIONS]; // for when an axis is mapped as a button
    bool gamepadAxisPressedPreviousFrame[INPUT_MAX_ACTIONS];
    bool anyGamepadButtonPressed;

    TouchPoint touchPoints[INPUT_MAX_TOUCH_POINTS];
    int touchCount;
    bool touchMode; // enabled when touch points are detected, disabled by any non-touch input
    bool touchButtonDown[INPUT_MAX_ACTIONS];
    bool touchButtonPressed[INPUT_MAX_ACTIONS];
    int touchButtonFrameActive[INPUT_MAX_ACTIONS];

    // generic input data
    bool anyKeyPressed;
    bool anyInputPressed;
    bool cancelled;
    bool mouseCancelled;
    float cancelTime;
} InputState;

typedef struct { // for auto rapid-fire input
    float triggerTime;
    float fireInterval;
    float heldTime;
    bool active;
} AutoRepeatSettings;

extern InputState input; // global declaration

// Prototypes
// ----------------------------------------------------------------------------

// Primary
void InitDefaultInputSettings(void); // Sets the default control settings and mappings
void AddInputActionGamepadButton(InputAction action, GamepadButton button);
void UpdateInputFrame(void);
void ProcessUserInput(InputPollFlag pollFlag); // Process all user inputs and actions for the current frame
void CancelMouseInput(void);
void CancelInputActions(void); // Cancel all user input actions for the current frame

// Input Actions
bool IsInputKeyModifier(KeyboardKey key);
bool IsInputActionDown(InputAction action);
bool IsInputActionPressed(InputAction action);
bool IsInputActionAxisDown(InputAction action);
bool IsInputActionAxisPressed(InputAction action);
bool IsInputActionMouseDown(InputAction action);
bool IsInputActionMousePressed(InputAction action);

// Touch
void SetTouchInputActionDown(InputAction action, bool buttonDown); // Set the touch input action to be pressed down or released this frame
void SetTouchPointButton(int index, int buttonIdx); // Set a touch point's current button id (currently used for touch screen analog stick, which probably needs a redesign/rewrite)

bool IsTouchPointTapped(int index); // Check if a touch point was tapped (touch equivalent for IsMouseButtonPressed)
bool IsTouchPointReleased(int index); // Check if a touch point was released (touch equivalent for IsMouseButtonReleased)
// bool IsTouchingButton(int index, int buttonId); // Check if a touch point is on a specific ui button
bool IsTouchingAnyButton(int index); // Check if a touch point is on any button

int CheckCollisionTouchCircle(Vector2 center, float radius); // Check if any touch points are within a circle, returns index to touch point or -1
int CheckCollisionTouchRec(Rectangle rec); // Check if any touch points are within a rectangle, returns index to touch point or -1

// Other
bool AutoRepeatShouldFire(AutoRepeatSettings *ar, bool fireCondition);


#endif // FROGGER_INPUT_HEADER_GUARD
