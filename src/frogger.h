// EXPLANATION:
// All the game logic, including how/when to draw to screen

#ifndef FROGGER_GAME_HEADER_GUARD
#define FROGGER_GAME_HEADER_GUARD

// Macros
// ----------------------------------------------------------------------------
#define GRID_RES_X 16 // playable game grid resolution
#define GRID_RES_Y 14
#define GRID_UNIT 40.0f // size of a grid square
#define GRID_WIDTH (GRID_UNIT*GRID_RES_X)
#define GRID_HEIGHT (GRID_UNIT*GRID_RES_Y)

#define BASE_SPEED (GRID_UNIT*1.5f)

// Types and Structures
// ----------------------------------------------------------------------------

typedef enum {
    SCREEN_LOGO, SCREEN_TITLE, SCREEN_GAMEPLAY
} ScreenState;

// typedef struct GameSounds {
//     Sound menu;
// } GameSounds;

// typedef struct GameTextures {
//     Texture ship;
// } GameTextures;

typedef enum {
    ENTITY_TYPE_FROG,
    ENTITY_TYPE_CAR,
    ENTITY_TYPE_LILYPAD,
    ENTITY_TYPE_LOG,
    ENTITY_TYPE_WALL,
    ENTITY_TYPE_WIN,
} EntityType;

typedef enum {
    ENTITY_FLAG_PLATFORM = (1 << 1),
    ENTITY_FLAG_MOVE = (1 << 2),
    ENTITY_FLAG_KILL = (1 << 3),
} EntityFlags;

typedef enum {
    ENTITY_MOVE_UP,
    ENTITY_MOVE_DOWN,
    ENTITY_MOVE_LEFT,
    ENTITY_MOVE_RIGHT
} EntityMoveDirection;

typedef struct {
    Rectangle rec;
    Vector2 position;
    Vector2 seekPos;
    Vector2 bufferPos;
    Color color;
    float speed;
    float radius;
    float platformMove;
    EntityType type;
    EntityFlags flags;
    bool isMoving;
    bool isWrapping;
    bool isMoveBuffered;
    bool isOnPlatform;
    bool isDrowned;
    bool isDead;
    bool isWin;
} Entity;

typedef struct {
    // General game data
    // ----------------------------------------------------------------------------
    ScreenState currentScreen;
    float frameTime;
    int frameCount;
    Camera2D camera;
    bool isPaused;
    bool isFullscreen;
    bool isInputDisabledFromResume;
    bool shouldExit;
    bool isDebugMode;

    // Frogger data
    // ----------------------------------------------------------------------------
    RaylibAssets assets;
    // GameSounds sounds;
    // GameTextures textures;

    int winCount;
    struct {
        Rectangle water;
        Rectangle grassTop, grassBottom;
        // Rectangle road;
    } background;

    Entity *entities;
    Entity *frog; // pointer to frog for convenience
    float deathTimer;
    float messageTimer;

    Vector2 grid[GRID_RES_X*GRID_RES_Y];
    Vector2 gridStart;
    Vector2 spawnPos;
} GameState;

extern GameState game; // global declaration

// Prototypes
// ----------------------------------------------------------------------------

// Initialization
void InitGameState(void); // Initialize game data and allocate memory for sounds
void CreateRow(EntityType type, int row, char *pattern, float speed); // create a row of entities (e.g. logs, cars)
                                                                      // pattern:
                                                                      // _ full unit space
                                                                      // . half unit space
                                                                      // o half width
                                                                      // O full width
void FreeGameState(void);

// Update & Draw
void UpdateGameFrame(void); // Updates all the game's data and objects for the current frame
void UpdateFrog(Entity *frog);
void UpdateHostile(Entity *hostile);
void UpdatePlatform(Entity *platform);
void UpdateWinZone(Entity *box);
void MoveEntity(Entity *e);
void DrawGameFrame(void); // Draws all the game's objects for the current frame

// Misc
Vector2 GetGridPosition(int row, int col);
void KillFrog(void);

#endif // FROGGER_GAME_HEADER_GUARD
