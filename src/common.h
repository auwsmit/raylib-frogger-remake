// EXPLANATION:
// Shared headers across the project

#ifndef FROGGER_COMMON_HEADER_GUARD
#define FROGGER_COMMON_HEADER_GUARD

// External Headers
// ----------------------------------------------------------------------------
#include "raylib.h"
#include "raymath.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h" // for dynamic arrays

#define ARENA_IMPLEMENTATION
#include "arena.h"

// Project Headers
// ----------------------------------------------------------------------------
#include "config.h"   // program config, e.g. window title/size, fps, vsync
#include "rl_utils.h" // raylib extra convenience

// Modules
#include "frogger.h"
#include "render.h"   // for rendering window and screen shader
#include "input.h"    // input actions and helpers
#include "logo.h"     // startup raylib logo animation
#include "ui.h"       // user interface


#endif // FROGGER_COMMON_HEADER_GUARD
