# --- Usage Notes -------------------------------------------------------------
#
# This makefile is used to build this project on Windows, Linux, and Mac.
#
# With no arguments, running `make` will build the game for Desktop with gcc.
# The executable/output file(s) will be in the repo directory.
#
# Below is a list of arguments you can use:
# `make CONFIG=RELEASE` --> optimized build, no debug files (debug is default)
# `make msvc`  --> use msvc/cl.exe to compile
# `make web`   --> compile to web assembly build
# `make clean` --> delete all previously generated build files
# `make run`   --> build and run desktop executable
#
# -----------------------------------------------------------------------------

# =============================================================================
# Project Config
# =============================================================================

# Default build
CONFIG ?= DEBUG
PLATFORM ?= DESKTOP

# Set compiler
ifeq ($(PLATFORM),DESKTOP)
    CC ?= gcc
else # PLATFORM,WEB
    CC := emcc
endif

# Source code to compile
SRC := src/main.c

# Dependencies
RAYLIB_DEP := deps/raylib
STB_DEP := deps/stb

# Output
ifeq ($(PLATFORM),DESKTOP)
    OUTPUT := frogger
    ifeq ($(OS),Windows_NT)
        EXTENSION  := .exe
    else
        EXTENSION  :=
    endif
else # PLATFORM,WEB
    OUTPUT := index
    EXTENSION  := .html
endif

# Linking flags
ifeq ($(OS),Windows_NT)
    LDFLAGS    := -lraylib -L"$(RAYLIB_DEP)/lib/windows-mingw" -lopengl32 -lgdi32 -lwinmm
else ifeq ($(shell uname -s),Linux)
    LDFLAGS    := -lraylib -L"$(RAYLIB_DEP)/lib/linux" -lGL -lm -lpthread -ldl -lrt -lX11
else ifeq ($(shell uname -s),Darwin) # macOS
    LDFLAGS    := -lraylib -L"$(RAYLIB_DEP)/lib/mac" -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Default compiler settings (e.g. gcc & clang)
CFLAGS_RELEASE := -O2
CFLAGS_DEBUG   := -g -O0
LDFLAGS_DEBUG  :=
CFLAGS         := -std=c99 -Wall -Wno-missing-braces -Wunused-result \
                  -Wextra -Wstrict-prototypes -Wfloat-conversion
CPPFLAGS       := -I"$(RAYLIB_DEP)" -I"$(STB_DEP)" -D_DEFAULT_SOURCE
PLATFORM_DEF   := -DPLATFORM_DESKTOP
OUTPUT_FLAG    := -o $(OUTPUT)$(EXTENSION)

# Compiler overrides
ifeq ($(CC),cl) # MSVC
    CFLAGS_RELEASE := /O2
    CFLAGS_DEBUG   := /Od /Zi
    CFLAGS         := /W3 /MD
    CPPFLAGS       := /I"$(RAYLIB_DEP)" /I"$(STB_DEP)" /D_DEFAULT_SOURCE
    LDFLAGS        := /link /LIBPATH:"$(RAYLIB_DEP)/lib/windows-msvc" \
                      raylib.lib gdi32.lib winmm.lib user32.lib shell32.lib
    LDFLAGS_DEBUG  := /DEBUG
    PLATFORM_DEF   := /DPLATFORM_DESKTOP
    OUTPUT_FLAG    := /Fe:$(OUTPUT)$(EXTENSION)
else ifeq ($(CC),emcc) # Emscripten
    CFLAGS_RELEASE := -Os
    CFLAGS_DEBUG   := $(CFLAGS_RELEASE)
    LDFLAGS        := -lraylib -L"$(RAYLIB_DEP)/lib/web" --shell-file shell.html --preload-file assets \
                      -sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sTOTAL_MEMORY=67108864 \
                      -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sEXPORTED_RUNTIME_METHODS=HEAPF32
    LDFLAGS_DEBUG  :=
    PLATFORM_DEF   := -DPLATFORM_WEB
endif

# Debug or Release build
ifeq ($(CONFIG),DEBUG)
    CFLAGS += $(CFLAGS_DEBUG)
    LDFLAGS += $(LDFLAGS_DEBUG)
else # CONFIG,RELEASE
    CFLAGS += $(CFLAGS_RELEASE)
endif

# Combine CFLAGS
CFLAGS += $(CPPFLAGS) $(PLATFORM_DEF)

# =============================================================================
# Targets
# =============================================================================

# let `make` know that these aren't files
.PHONY: all msvc web run clean

# Default: Compile all files for desktop
all:
	$(CC) $(CFLAGS) $(SRC) $(OUTPUT_FLAG) $(LDFLAGS)

# Build with MSVC cl.exe
msvc:
	$(MAKE) CC=cl
	@rm -f *.obj

web:
	$(MAKE) PLATFORM=WEB

run:
	$(MAKE) && ./$(OUTPUT)$(EXTENSION)

# Clean up generated build files
clean:
	@rm -rf $(OUTPUT)$(EXTENSION) \
	        index.html index.js index.wasm index.data \
	        $(OUTPUT).ilk $(OUTPUT).pdb vc140.pdb *.rdi
	@echo "Make build files cleaned"
