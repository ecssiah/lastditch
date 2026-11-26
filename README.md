# Last Ditch
The Garden of Civilization

![Wolf](assets/textures/face/WolfImage.png) ![Eagle](assets/textures/face/EagleImage.png) ![Bear](assets/textures/face/BearImage.png) ![Lion](assets/textures/face/LionImage.png)

## Building

Last Ditch is written in ANSI C89. On Debian, install the dependencies listed
in [the build guide](build.md#debian-development-with-gnu-make), then run:

```sh
make -j
make check
./build-make/lastditch
```

The [build guide](build.md) also covers the vendored CMake build, reference
tables, shaders, and tests.

## Editor navigation

To generate an Emacs tag table, run this from the repository root:

```sh
find src tests data -type f \( -name '*.c' -o -name '*.h' \) | etags -o TAGS -
```

Open `TAGS` with `M-x visit-tags-table`.

## Controls

| Input | Action |
| --- | --- |
| `W` `A` `S` `D` | Move |
| Mouse | Look |
| `Space` | Jump |
| `Tab` | Switch between the judge and a free-flying camera |
| `E` / `Q` | Fly up / down (free camera) |
| `Esc` | Quit |

## Documentation

| Document | Contents |
| --- | --- |
| [build.md](build.md) | How the project is built and tested. |
| [docs/data_model.md](docs/data_model.md) | How the simulation's data is organised: tables, keys, indexes, reference tables and the systems that work on them. |
| [docs/documentation_standard.md](docs/documentation_standard.md) | The rules every source comment follows, and the tool that checks them. |
