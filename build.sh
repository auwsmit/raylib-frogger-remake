#!/usr/bin/env bash
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

# Credit to RAD Debugger for many pointers and ideas

# Usage Notes
# -----------------------------------------------------------------------------
#
# This is the build script for this project on Linux.
#
# It can be used in two main ways:
# 1. `./build` to simply compile the game with a C compiler
# 2. `./build cmake` to config and run the CMake build system
# Either way, you will find the output executable in the project directory.
#
# Additional arguments can be given to choose the compiler, compiler settings,
# or to clean up old build files.
#
# Below is a non-exhaustive list of arguments you can use:
# `build release` -> optimized build, no debug symbols
# `build clang`   -> use clang compiler
# `build web`     -> compile to web assembly with emscripten
# `build clean`   -> delete old generated build files (excluding CMake)
# CMake build:
# `build cmake`       -> config and build using CMake
# `build cmake web`   -> compile to web assembly with emscripten
# `build cmake clean` -> delete CMake's old build files
#
# Note: CMake will automatically download and build raylib if needed
#
# -----------------------------------------------------------------------------

# Project Config
# -----------------------------------------------------------------------------
output=frogger
source_code=src/main.c

raylib_dep=deps/raylib
raylib_dep=deps/stb

cmake_build_dir=build

# Script Entry Point
main()
{
    script_unpack_args "$@"
    pushd "$script_dir" > /dev/null
    if [[ "$cmake" == 1 ]]; then
        script_choose_cmake_lines
        script_cmake_config_and_build
    else
        script_choose_simple_lines
        script_simple_build
    fi
    popd
}

# Unpack Arguments
script_unpack_args()
{
    for arg in "$@"; do eval "$arg=1"; done
    if [[ "$clean" == 1 ]]; then
        echo "[clean mode]" && script_build_cleanup
        exit 0
    fi
    if [[ "$web" != 1     ]]; then release=1; fi
    if [[ "$release" != 1 ]]; then debug=1; fi
    if [[ "$release" == 1 ]]; then debug=0   && echo "[release mode]"; fi
    if [[ "$debug" == 1   ]]; then release=0 && echo "[debug mode]"; fi
    if [[ "$cmake" == 1   ]]; then
        echo "[cmake build]"
        if [[ "$web" != 1 ]]; then gcc=1 && echo "[gcc compile]"; fi
    else
        simple_build=1 && echo "[simple build]"
        if [[ "$clang" == 1 ]]; then gcc=0 && echo "[clang compile]"; fi
        if [[ "$web" == 1   ]]; then clang=0; fi
        if [[ "$web" != 1 && "$clang" != 1 ]]; then
            gcc=1 && echo "[gcc compile]"
        fi
    fi
    if [[ "$web" == 1 ]]; then echo "[web compile]"; fi
}

# Define and Choose CMake Lines
script_choose_cmake_lines()
{
    # Line Definitions
    cmake_config_desktop="cmake -B \"$cmake_build_dir/desktop\" -DPLATFORM=Desktop"
    cmake_build_desktop="cmake --build \"$cmake_build_dir/desktop\""
    cmake_config_web="emcmake cmake -B \"$cmake_build_dir/web\" -DPLATFORM=Web"
    cmake_build_web="emmake make -C \"$cmake_build_dir/web\""

    # Choose Lines
    if [[ "$web" == 1 ]]; then
        output_dir=$cmake_build_dir/web
        cmake_config_cmd=$cmake_config_web
        cmake_build_cmd=$cmake_build_web
    else
        output_dir=$cmake_build_dir/desktop
        cmake_config_cmd=$cmake_config_desktop
        cmake_build_cmd=$cmake_build_desktop
    fi
    if [[ "$release" == 1 ]]; then cmake_config_flags='-DCMAKE_BUILD_TYPE=Release'; fi
    if [[ "$debug" == 1 ]]; then cmake_config_flags='-DCMAKE_BUILD_TYPE=Debug'; fi
}

# Define and Choose Compile/Link Lines
script_choose_simple_lines()
{
    # Line Definitions
    cc_common="-I\"$raylib_dep\" -I\"$stb_dep\" -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Wextra -Wstrict-prototypes -Wfloat-conversion"
    cc_debug='-g -O0'
    cc_release='-O2'
    cc_platform='-DPLATFORM_DESKTOP'
    cc_link="-lraylib -L\"$raylib_dep/lib/linux\" -lGL -lm -lpthread -ldl -lrt -lX11"
    cc_out='-o'

    web_release='-Os'
    web_platform='-DPLATFORM_WEB'
    web_link="-lraylib -L\"$raylib_dep/lib/web\" --shell-file shell.html --preload-file assets -sUSE_GLFW=3 -sTOTAL_MEMORY=67108864 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sEXPORTED_RUNTIME_METHODS=HEAPF32"

    # Choose Lines
    if [[ "$gcc" == 1   ]]; then compile="gcc $cc_common"; fi
    if [[ "$clang" == 1 ]]; then compile="clang $cc_common"; fi
    if [[ "$web" == 1 ]]; then compile="emcc $cc_common"; fi

    if [[ "$web" != 1 ]]; then # gcc/clang
        compile_platform="$cc_platform"
        compile_link="$cc_link"
        compile_debug="$cc_debug"
        compile_out="$cc_out $output"
        compile_release="$cc_release"
    else # web, emscripten
        compile_platform="$web_platform"
        compile_link="$web_link"
        compile_out="$cc_out index.html"
        compile_release="$web_release"
        compile_debug="$web_release";
    fi

    if [[ "$debug" == 1   ]]; then compile="$compile $compile_debug"; fi
    if [[ "$release" == 1 ]]; then compile="$compile $compile_release"; fi
    compile="$compile $compile_platform"
}

script_cmake_config_and_build()
{
    echo "$cmake_config_cmd $cmake_config_flags"
    echo "$cmake_build_cmd $cmake_build_flags"
    eval $cmake_config_cmd $cmake_config_flags
    eval $cmake_build_cmd $cmake_build_flags
    if [[ "$web" == 1 ]]; then
        rm -f "index.html" "index.js" "index.wasm" "index.data"
        cp "$output_dir/index.html" .
        cp "$output_dir/index.js" .
        cp "$output_dir/index.wasm" .
        cp "$output_dir/index.data" .
    else
        rm -f "$output"
        cp "$output_dir/$output" .
    fi
}

script_simple_build()
{
    echo "$compile $source_code $compile_out $compile_link"
    eval $compile $source_code $compile_out $compile_link
}

script_build_cleanup()
{
    pushd "$script_dir" > /dev/null
    if [[ "$cmake" == 1 ]]; then
        rm -rf $output build/
        echo "CMake build files cleaned"
    else
        rm -rf index build_web/ index.html index.js index.wasm index.data
        echo "Build files cleaned"
    fi
    popd
}

main "$@"
