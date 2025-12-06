// EXPLANATION:
// All the game logic, including how/when to draw to screen

#ifndef FROGGER_GAME_HEADER_GUARD
#define FROGGER_GAME_HEADER_GUARD

// Macros
// ----------------------------------------------------------------------------
#define GRID_HEIGHT 18
#define GRID_WIDTH 32
#define SQUARE_SIZE ((float)VIRTUAL_WIDTH/GRID_WIDTH)

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
    Vector2 grid[GRID_WIDTH*GRID_HEIGHT];
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
void DrawEntities(void);

Vector2 GetGridPosition(int row, int col);

#endif // FROGGER_GAME_HEADER_GUARD
