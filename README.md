# Last Ditch
The Garden of Civilization

![Wolf](assets/textures/face/WolfImage.png) ![Eagle](assets/textures/face/EagleImage.png) ![Bear](assets/textures/face/BearImage.png) ![Lion](assets/textures/face/LionImage.png)

## Building

Last Ditch is written in ANSI C89 and built with CMake as a single
translation unit (a Unity build) on top of the vendored SDL3 and SDL3_ttf.

```sh
python3 tools/bootstrap.py
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target shaders
cmake --build build
ctest --test-dir build
./build/lastditch
```

[docs/build.md](docs/build.md) explains the build in detail: the language
dialect, how the Unity build works, the generated code, the shaders and the
tests.

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
| [docs/build.md](docs/build.md) | How the project is built and tested. |
| [docs/data_model.md](docs/data_model.md) | How the simulation's data is organised: tables, keys, indexes, reference tables and the systems that work on them. |
| [docs/documentation_standard.md](docs/documentation_standard.md) | The rules every source comment follows, and the tool that checks them. |
