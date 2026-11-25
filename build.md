# Building Last Ditch

## Debian development with GNU Make

Install the compiler, SDL development libraries and the shader compiler:

```sh
sudo apt install build-essential pkgconf libsdl3-dev libsdl3-ttf-dev libsdl3-image-dev glslang-tools
make -j
make check
./build-make/lastditch
```

The Make build uses GNU Make 4.3 or newer. It links against the installed
SDL3 and SDL3_ttf, plus SDL3_image for PNG support on SDL 3.2. It compiles
the C files separately in GNU C90 mode and puts the game, assets, data and
Vulkan shaders in `build-make/`. The default target updates runtime files
independently of linking.

Use `make shaders` to compile just the SPIR-V shaders.

Use a distinct `BUILD_DIR` when switching toolchains or build flags. The Make build currently supports
Linux. Platform-specific link and shader rules live in `make/linux.mk`.

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
