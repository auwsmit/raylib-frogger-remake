// EXPLANATION:
// For managing the user interface

#ifndef FROGGER_UI_HEADER_GUARD
#define FROGGER_UI_HEADER_GUARD

// Macros
// ----------------------------------------------------------------------------

#define UI_MENU_AMOUNT 3 // size of the menu array

// UI appearance
#define UI_TITLE_FONT_SIZE    80
#define UI_MENU_FONT_SIZE     50
#define UI_SETTINGS_FONT_SIZE 50
#define UI_TITLE_BUTTON_WIDTH 300
#define UI_PAUSE_BUTTON_WIDTH 500
#define UI_SETTINGS_BUTTON_WIDTH 500
#define UI_FONT_SIZE_CENTER   140   // center of screen font size
#define UI_FONT_SIZE_EDGE     75    // top of screen font size
#define UI_BORDERED_DEFAULT   true
#define UI_BORDER_THICKNESS   3
#define UI_TRANSPARENCY       0.50f
// UI selection appearance
#define UI_CURSOR_SIZE        30.0f // cursor triangle size
#define UI_SELECT_GROWTH_MULT 1.2f
#define UI_SELECT_COLOR       GREEN

// Spacing / padding
#define UI_TITLE_TOP_PADDING UI_TITLE_FONT_SIZE // space from the top of the screen
#define UI_BUTTON_SPACING    UI_MENU_FONT_SIZE/2  // space between each button
#define UI_EDGE_PADDING      30  // space from screen edges

// Virtual input
#define UI_STICK_RADIUS 70.0f
#define UI_DPAD_WIDTH   70.0f
#define UI_INPUT_RADIUS 40.0f
#define UI_INPUT_PADDING 75.0f

// Types and Structures
// ----------------------------------------------------------------------------

typedef enum {
    UI_MENU_TITLE, UI_MENU_SETTINGS, UI_MENU_PAUSE, UI_MENU_NONE
} UiMenuState;

typedef enum { // style for menu selection
    UI_SELSTYLE_CURSOR    = (1 << 0),
    UI_SELSTYLE_HL_RECT   = (1 << 1),
    UI_SELSTYLE_HL_TEXT   = (1 << 2),
    UI_SELSTYLE_UNDERLINE = (1 << 3),
    UI_SELSTYLE_GROW      = (1 << 4)
} UiSelectionStyle;

typedef enum {
    UI_ALIGN_LEFT     = 0, UI_ALIGN_TOP    = 0,
    UI_ALIGN_CENTER   = 1, UI_ALIGN_MIDDLE = 1,
    UI_ALIGN_RIGHT    = 2, UI_ALIGN_BOTTOM = 2,
    UI_ALIGN_DISABLED = 3
} UiAlignment;

typedef enum { UI_INPUT_ON_PRESS, UI_INPUT_ON_HOLD } UiInputTrigger;

// Callback functions for interactive elements
typedef void (*UiActionFunc)(void);
typedef void (*UiSetFloatFunc)(float setValue, void *slider);
typedef float (*UiGetFloatFunc)(void);
typedef bool (*UiGetBoolFunc)(void);

typedef struct {
    Sound menu;
} UiSounds;

typedef struct {
    Texture atlas;
    Rectangle analogBase;
    Rectangle analogStick;
    Rectangle dpad;
    Rectangle pause;
} UiTextures;

typedef struct {
    Color color;
    Vector2 position;
    int fontSize;
    const char *text;
} UiText;

typedef struct {
    Texture sprite;
    Rectangle rec;
    Vector2 position;
    Vector2 growPos;
    float radius;
    UiGetBoolFunc getValue;
} UiCheckbox;

typedef struct {
    Texture sprite;
    Rectangle rec;
    float min, max;
    float increment;
    UiSetFloatFunc setValue;
    UiGetFloatFunc getValue;
    bool active;
} UiSlider;

typedef struct {
    Texture *sprite;
    Vector2 position;
    Vector2 growPos;
    Rectangle rec, spriteRec;
    Color color;
    float spriteScale;
    float width, height;
    float radius;
    UiActionFunc onClick;
    UiSlider *slider;
    UiCheckbox *checkbox;
    int inputActionId;
    int fontSize;
    bool bordered;
    bool clicked;
    const char *text;
} UiButton;

typedef struct {
    UiText *text;
    UiButton *buttons;
    float buttonWidth;
    int spacing;
    int fontSize;
    UiSelectionStyle selectStyleFlags; // (e.g. cursor, underline, highlight)
} UiMenu;

typedef struct {
    Texture *sprite;
    Rectangle spriteBaseRec,  spriteStickRec;
    Vector2 centerPos,    stickPos;
    float   centerRadius, stickRadius;
    int lastTouchId;
    bool active;
    bool enabled;
} UiAnalogStick;

typedef enum {
    UI_DPAD_UP, UI_DPAD_DOWN, UI_DPAD_LEFT, UI_DPAD_RIGHT
} UiDPadDirections;

typedef struct {
    Texture *sprite;
    Rectangle button[4]; // up, down, left, right
    Rectangle rec, spriteRec;
    Vector2 position;
    float width;
    float spriteScale;
    int inputActionId[4];
    bool clicked[4];
    bool enabled;
} UiDPad;

typedef struct { // Virtual touchscreen input
    UiButton pause;
    UiAnalogStick stick;
    UiDPad dpad;
} UiGamepad;

typedef struct {
    // Primary data
    Arena arena;
    RaylibAssets assets;
    UiTextures textures;
    UiSounds sounds;
    Camera2D camera;
    UiMenu menus[UI_MENU_AMOUNT];
    UiMenuState currentMenu;
    int selectedId;
    int mouseHoverId;
    bool firstFrame;
    bool lastSelectWithMouse;
    bool preventMouseClick;
    UiGamepad gamepad; // touchscreen gamepad

    // Used during menu creation
    UiMenuState initMenu; // menu to add buttons to
    UiAlignment hAlign, vAlign; // alignment for new buttons
    UiButton *lastButtonCreated;

    // Other / optional
    UiText timedMessage;
    int timedMessageLength;
    float messageTimer;
    float actionCooldownTimer;
    float textFade; // for fade in and out animation
    float textFadeTimeElapsed;
} UiState;

extern UiState ui; // global declaration

// Prototypes
// ----------------------------------------------------------------------------

// Initialize
void InitUiState(void); // Initializes the title screen and allocates memory for menu buttons
UiButton InitUiButton(char *text, UiActionFunc actionFunc, float x, float y, float buttonWidth, int fontSize); // creates a default UI button
void CreateUiText(char *text, float x, float y, int fontSize);
UiButton InitUiInputButton(char *text, int inputActionId, float textPosX, float textPosY, float radius);
void CreateUiCheckbox(UiGetBoolFunc getValue);
void CreateUiSlider(UiSetFloatFunc setValue, UiGetFloatFunc getValue, float minValue, float maxValue, float increment);
UiButton *CreateUiMenuButton(char *text, UiActionFunc actionFunc, float x, float y); // Initializes a button within a menu
void CreateUiMenuButtonRelative(char* text, UiActionFunc actionFunc); // Initializes a button within a menu relative to the last menu button
void SetUiAlignMode(UiAlignment hAlign, UiAlignment vAlign);
// void FreeUiState(void); // Frees memory for all menu buttons

// Update / User Input
void UpdateUiFrame(void); // Updates the menu for the current frame
void UpdateUiMenuTraverse(void); // Updates the cursor for movement by user input
void UpdateUiButtonSelect(UiButton *button); // Selects a button by user input
void UpdateUiSliderSelect(UiSlider *slider);
void ChangeUiMenu(UiMenuState newMenu); // Change from one menu to another

// Touch screen virtual gamepad
void UpdateUiGamepadRender(void);
void UpdateUiTouchInput(UiButton *button, UiInputTrigger onPressOrHold); // Updates virtual input from button
void UpdateUiAnalogStick(UiAnalogStick *stick);
void UpdateUiDPad(UiDPad *dpad);

// Collision
bool IsMouseWithinUiButton(UiButton *button);
int IsTouchWithinUiButton(UiButton *button);

// Draw
void DrawUiFrame(void); // Draws all the UI buttons for the current frame
void DrawUiGamepad(void);
void DrawUiButton(UiButton *button);
void DrawUiCursor(UiButton *selectedButton); // Draw the cursor at the given button
void DrawUiInputButton(UiButton *button); // Draw touch input button
void DrawUiDPad(UiDPad *dpad); // Draw touch directional pad
void DrawUiAnalogStick(UiAnalogStick *stick); // Draw touch analog stick
void DrawLives(void);

void DrawDebugInfo(void);

// Other
void SetTimedMessage(char *message, int fontSize, float time);

#endif // FROGGER_UI_HEADER_GUARD
