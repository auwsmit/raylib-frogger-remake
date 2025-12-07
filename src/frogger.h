// EXPLANATION:
// All the game logic, including how/when to draw to screen

#ifndef FROGGER_GAME_HEADER_GUARD
#define FROGGER_GAME_HEADER_GUARD

// Macros
// ----------------------------------------------------------------------------
#define GRID_RES_X 14 // playable game grid resolution
#define GRID_RES_Y 13
#define GRID_UNIT 50.0f // size of a grid square
#define GRID_WIDTH GRID_UNIT*GRID_RES_X
#define GRID_HEIGHT GRID_UNIT*GRID_RES_Y

// Types and Structures
// ----------------------------------------------------------------------------

typedef enum ScreenState {
    SCREEN_LOGO, SCREEN_TITLE, SCREEN_GAMEPLAY
} ScreenState;

// typedef struct GameSounds {
//     Sound menu;
// } GameSounds;

// typedef struct GameTextures {
//     Texture ship;
// } GameTextures;

typedef enum EntityType {
    ENTITY_TYPE_FROG,
    ENTITY_TYPE_CAR,
    ENTITY_TYPE_LILYPAD,
    ENTITY_TYPE_LOG,
} EntityType;

typedef struct Point {
    int x;
    int y;
} Point;

typedef enum EntityMoveDirection {
    ENTITY_MOVE_UP,
    ENTITY_MOVE_DOWN,
    ENTITY_MOVE_LEFT,
    ENTITY_MOVE_RIGHT
} EntityMoveDirection;

typedef struct Entity {
    Rectangle rect;
    Vector2 position;
    Vector2 seekPos;
    Vector2 bufferPos;
    Point gridIndex;
    Color color;
    float speed;
    float radius;
    EntityType type;
    bool moving;
    bool moveInput;
    bool moveBuffered;
} Entity;

typedef struct GameState {
    // General game data
    float frameTime;
    ScreenState currentScreen;
    int frameCount;
    Camera2D camera;
    bool paused;
    bool fullscreen;
    bool resumeInputCooldown;
    bool gameShouldExit;
    bool debugMode;

    // Frogger data
    RaylibAssets assets;
    // GameSounds sounds;
    // GameTextures textures;
    struct {
        Entity frog;
        Entity *cars;
        Entity *lilypads;
        Entity *logs;
    } entities;
    Vector2 grid[GRID_RES_X*GRID_RES_Y];
    Vector2 gridStart;
} GameState;

extern GameState game; // global declaration

// Prototypes
// ----------------------------------------------------------------------------

// Initialization
void InitGameState(ScreenState screen); // Initialize game data and allocate memory for sounds
// void InitNewLevel(int newLevel);

// Update & Draw
void UpdateGameFrame(void); // Updates all the game's data and objects for the current frame
void UpdateFrog(Entity *frog);
void UpdateLilypad(Entity *lilypad);
void UpdateCar(Entity *car);
void DrawGameFrame(void); // Draws all the game's objects for the current frame

Vector2 GetGridPosition(int row, int col);

#endif // FROGGER_GAME_HEADER_GUARD
