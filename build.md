# Building Last Ditch

## Debian development with GNU Make

Install the compiler, SDL development libraries and the shader compiler:

```sh
sudo apt install build-essential pkgconf libsdl3-dev libsdl3-ttf-dev libsdl3-image-dev glslang-tools
make -j build
make check
./build/lastditch
```

The Make build uses GNU Make 3.81 or newer. It links against the installed
SDL3 and SDL3_ttf, plus SDL3_image for PNG support on SDL 3.2. It compiles
the C files separately in GNU C90 mode and puts the game, assets, data and
Vulkan shaders in `build/`. The default target updates runtime files
independently of linking.

Use `make shaders` to compile just the SPIR-V shaders.

## macOS development with GNU Make

Install Apple's Command Line Tools and the Homebrew dependencies, then build
and run on the Mac:

```sh
xcode-select --install
brew install pkgconf sdl3 sdl3_ttf glslang spirv-cross
make -j build
make check
./build/lastditch
```

Run the first command only if the Command Line Tools are missing. Use the
`make` bundled with macOS. If building against SDL 3.2, also install
`sdl3_image`.

If `pkg-config` cannot find `sdl3-ttf` after installation, use Homebrew's
`pkg-config` and add the SDL3_ttf formula's metadata directory to its search path:

```sh
brew install pkgconf sdl3_ttf
export PKG_CONFIG_PATH="$(brew --prefix sdl3_ttf)/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
"$(brew --prefix pkgconf)/bin/pkg-config" --modversion sdl3-ttf
make -j build PKG_CONFIG="$(brew --prefix pkgconf)/bin/pkg-config"
```

The build detects macOS automatically and produces a native executable for
the installed toolchain. It converts the HLSL shaders to Metal Shading
Language and stages them with the assets and data under `build/`. Use
`make shaders` to generate just the Metal shaders.

Use a distinct `BUILD_DIR` when switching platforms, toolchains or build flags.

## Reference tables

The C preprocessor reads three plain-text files in `data/`. Row order defines
the numeric IDs and must stay stable for existing rows.

| File | Row format |
| --- | --- |
| `nation_types.def` | `NATION(ID, name, texture)` |
| `face_types.def` | `FACE(ID, texture)` |
| `block_types.def` | `BLOCK(ID, name, FACE)` for a single texture or `BLOCK_FACES(ID, name, EAST, WEST, NORTH, SOUTH, UP, DOWN)` for six textures |

IDs and face references use uppercase identifiers. Names and texture
filenames use lowercase names without extensions. Add a 64×64 PNG under
`assets/textures/block/` for each new face texture, then rebuild.
