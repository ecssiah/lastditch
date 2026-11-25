# Building Last Ditch

## Quick start

```sh
python3 tools/bootstrap.py              # once: creates tools/.venv and installs PyYAML
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target shaders    # once, and whenever a shader changes
cmake --build build                     # builds the game and the test program
ctest --test-dir build                  # runs the tests and the source checks
./build/lastditch
```

On Windows use `py -3 tools/bootstrap.py`. The shader step must run before
the game is built for the first time, because the build copies `assets/`
(including `assets/shaders/compiled/`) next to the executable.

## Requirements

| What | Why |
| --- | --- |
| CMake 3.20 or newer and a build tool (Ninja or Make) | Build system. |
| A C compiler: GCC or Clang | The warning flags in `CMakeLists.txt` are GCC/Clang flags. |
| Python 3 | Code generation, shader compilation and the source checks. `tools/bootstrap.py` creates the virtual environment `tools/.venv` that CMake uses. |
| `spirv-cross`, and `dxc` or `glslangValidator` | Only for the `shaders` target. |
| The development packages SDL needs on your platform | SDL3 and SDL3_ttf are vendored under `vendor/` and built from source as static libraries. On Linux this means the X11 and/or Wayland headers; see `vendor/SDL3-3.4.14/docs`. |

The project enables a single language in CMake: C (plus Objective-C on
macOS, which SDL's platform layer needs).

## The language: ANSI C89 plus `//`

The game (`src/`) and its tests (`tests/`) are written in ANSI C89 (ISO C90).
Exactly one language extension is used: the single line `//` comment. The
code is therefore compiled as GNU C90 (`-std=gnu90`, CMake's `C_STANDARD 90`
with `C_EXTENSIONS ON`), the C89 dialect in which GCC and Clang accept `//`.

In practice this means, among other things:

* declarations only at the start of a block; no declarations in `for`;
* no designated initialisers, compound literals, variable length arrays,
  `inline`, `restrict`, `long long` or variadic macros;
* aggregate initialisers contain only constant expressions;
* only the C89 standard library: the math functions are the `double` ones
  (`src/core/geometry/scalar.c` wraps them for `f32`), and there is no
  `snprintf`, `stdint.h` or `stdbool.h`.

Three things C89 lacks are taken from SDL, which the game depends on anyway:

| Need | Taken from |
| --- | --- |
| Fixed-width integers, including the 64-bit types used for time stamps and the step counter | `Uint8` ... `Sint64` from `SDL_stdinc.h`, aliased in `src/core/types.h` |
| Bounded string formatting | `SDL_snprintf`, `SDL_vsnprintf`, `SDL_strlcpy` |
| File system access (directories, whole-file reads, the executable's location) | `SDL_CreateDirectory`, `SDL_GetPathInfo`, `SDL_LoadFile`, `SDL_GetBasePath` |

The `c89` test **proves** the dialect claim rather than trusting it:
`tools/python/check_c89.py` copies the sources, mechanically rewrites every
`//` comment as `/* */`, and compiles each file with
`-std=c89 -pedantic-errors -Wall -Wextra`. Anything that is not strict C89 –
another extension, a C99 feature, a call to a function C89 does not declare –
fails the test.

Regular builds add `-Wdeclaration-after-statement`, `-Wstrict-prototypes`,
`-Wold-style-definition`, `-Wmissing-prototypes` and `-Wvla` to `-Wall
-Wextra`, so most violations are already reported while compiling.

The vendored libraries are not C89 and are compiled with their own settings
(C17, as before); the `CMAKE_C_STANDARD` lines at the top of `CMakeLists.txt`
apply to them only. Their headers are included as system headers so that the
game's warning flags do not report on third-party code.

## The Unity build

The game is built as a **Unity build**: instead of compiling each `.c` file
into its own object file, the compiler sees the whole program as a single
translation unit.

The mechanism is CMake's: the target property `UNITY_BUILD ON` makes CMake
generate one source file,
`build/CMakeFiles/lastditch.dir/Unity/unity_0_c.c`, that contains nothing but
an `#include` of every `.c` file listed for the target in `CMakeLists.txt`,
in the order they are listed. That file is what gets compiled.
`UNITY_BUILD_BATCH_SIZE 0` tells CMake not to split the sources into several
batches, so there is exactly one translation unit per executable. The test
program `lastditch_tests` is built the same way.

What this buys: a full rebuild of the game's own code is one compiler
invocation, and the optimiser sees every function and every table at once.

What it asks of the code – every source file must be written so that it
works both when it is compiled alone and when it shares a translation unit
with all the others:

1. **Every file includes what it uses.** A `.c` file includes its own header
   first and then every header it needs; it never relies on an earlier file
   in the Unity order. All headers have include guards.
2. **File-private names are unique across the program.** A `static` function
   or variable, a private type and a private macro are visible to every file
   that comes later in the Unity file. All names therefore start with their
   module's name (`physics_resolve_axis_collisions`, `RENDER_PATH_CAPACITY`).
3. **Short private macros are removed again.** Where a file defines
   short-named helper macros for readability (the row shorthands in
   `src/app/data/blueprint.c`), it `#undef`s them at its end.
4. **Functions shared between files are declared in a header** and are not
   `static`; functions used by one file only are `static`.

Rule 1 is easy to break without noticing in a Unity build, so there is a
switch to check it:

```sh
cmake -S . -B build-separate -DLASTDITCH_UNITY_BUILD=OFF
cmake --build build-separate
```

This compiles every file on its own (with the same flags) and fails on any
missing include or clashing definition.

To add a source file, add it to `LASTDITCH_SIMULATION_SOURCES` (code that
runs without a window, shared with the tests) or
`LASTDITCH_APPLICATION_SOURCES` (the renderer and the entry point) in
`CMakeLists.txt`, and its header to `LASTDITCH_HEADERS`.

## Generated code

Two reference tables are authored as YAML and turned into C at build time:

| Input | Generator | Output (in `build/generated/`) |
| --- | --- | --- |
| `data/block_data.yml` | `tools/python/generate_block_data.py` | `block_data.generated.h`, `block_data.generated.c` |
| `data/actor_data.yml` | `tools/python/generate_actor_data.py` | `actor_data.generated.h`, `actor_data.generated.c` |

The header declares the key enum (for example `BLOCK_TYPE_SMOOTH1`), the
storage type for keys in columns (`BlockType`, the narrowest unsigned type
that fits) and the tables; the source file holds the rows. Both are part of
the Unity translation unit like any other source file. The generators
validate the YAML and only rewrite their outputs when the content changes.

To add a block: add its face types and the block to `data/block_data.yml`,
add a 64×64 PNG per new face type to `assets/textures/block/`, and rebuild.

## Shaders

The HLSL sources in `assets/shaders/` are compiled by
`tools/python/compile_shaders.py` into `assets/shaders/compiled/` as SPIR-V
(Vulkan), MSL (Metal) and, when `dxc` is available, DXIL (Direct3D 12):

```sh
cmake --build build --target shaders
```

The compiled shaders are not checked in and the `shaders` target is not part
of the default build, because it needs the external shader tools.

## What the build produces

```
build/
  lastditch              the game
  lastditch_tests        the test program
  assets/, data/         copied from the source tree after every link of the game
  generated/             the generated reference tables
```

At run time the game loads its assets relative to the directory of the
executable, so it can be started from any working directory. It writes a log
file per run to `logs/` under the working directory (and, when started from
inside a repository checkout but outside its root, a copy to `logs/` at the
repository root).

## Tests and checks

`ctest --test-dir build` runs three tests:

| Test | What it does |
| --- | --- |
| `simulation` | Runs `lastditch_tests`: the table primitives, random numbers and geometry; the data tables, indexes and reference tables; world generation (invariants plus recorded values for the world of the built-in seed); population, physics, scheduling and player control; HUD labels, the debug overlay, sector meshing and model loading. Needs no window and no GPU. |
| `documentation` | Runs `tools/python/check_documentation.py` over `src/`, `tests/` and the generated files; see [documentation_standard.md](documentation_standard.md). |
| `c89` | Runs `tools/python/check_c89.py`; see above. |

The renderer itself (`src/app/render.c`) is not covered by automated tests;
it needs a GPU device.

## Options

| CMake option | Default | Effect |
| --- | --- | --- |
| `CMAKE_BUILD_TYPE` | *(none)* | `Debug` keeps `assert` active and enables GPU validation; `Release` defines `NDEBUG`. |
| `LASTDITCH_UNITY_BUILD` | `ON` | `OFF` compiles every source file separately. |
| `LASTDITCH_BUILD_TESTS` | `ON` | `OFF` skips the test program and the `ctest` registrations. |
