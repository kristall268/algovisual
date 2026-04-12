# algovisual

Interactive algorithm visualizer built on top of Vulkan, SDL3 and Dear ImGui.

The project is structured as a small engine (`av_engine`) with a runtime scene
system, on top of which the main application renders sorting, graph and
2D-physics scenes that can be switched at runtime.

> **Status:** early development. Stage 1 (window + Vulkan + ImGui) is working;
> scene manager, custom 2D renderer and concrete visualizations are next.

---

## Features

- Vulkan 1.3 renderer (validation layers in Debug)
- SDL3 windowing and input
- Dear ImGui for in-app controls
- Hot-swappable scenes (`IScene` interface)
- spdlog-based logging
- GLSL → SPIR-V shader compilation at build time
- CMake presets for `debug` / `release`
- clangd + clang-format + LLDB integration in VS Code

## Project layout

```
algovisual/
├── engine/              # av_engine static library — core code
│   ├── include/av/      # public headers (consumers see <av/...>)
│   └── src/             # implementation, mirrors include layout
├── app/                 # main executable, registers scenes
├── examples/            # small standalone demos using the engine
├── tests/               # unit tests (opt-in via AV_BUILD_TESTS)
├── assets/
│   ├── shaders/         # .vert/.frag, compiled to .spv at build time
│   ├── fonts/
│   └── icons/
├── external/
│   └── imgui/           # git submodule (docking branch)
├── cmake/               # helper modules (shader compilation, asset copy)
└── CMakePresets.json
```

## Requirements

The project targets **Linux** (primary) and **Windows** (best-effort).
macOS is not currently supported (planned via MoltenVK).

### Linux (Arch)

```bash
sudo pacman -S --needed base-devel git cmake ninja pkgconf \
    clang llvm lldb \
    vulkan-headers vulkan-icd-loader vulkan-validation-layers vulkan-tools \
    glslang spirv-tools \
    mesa vulkan-radeon \
    sdl3 spdlog fmt
```

For NVIDIA GPUs replace `vulkan-radeon` with `nvidia-utils`.
For Intel — `vulkan-intel`.

### Linux (Debian/Ubuntu)

`sdl3` requires Ubuntu 24.10+ or building from source.

```bash
sudo apt install build-essential git cmake ninja-build pkg-config \
    clang clangd lldb \
    libvulkan-dev vulkan-validationlayers-dev vulkan-tools \
    glslang-tools spirv-tools \
    libsdl3-dev libspdlog-dev libfmt-dev
```

### Windows

Currently unmaintained. Last known working setup used vcpkg (`sdl3`, `spdlog`)
plus the official Vulkan SDK.

## Build

Clone with submodules:

```bash
git clone --recurse-submodules https://github.com/kristall268/algovisual.git
cd algovisual
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --recursive
```

Configure and build:

```bash
cmake --preset debug
cmake --build --preset debug
```

Run:

```bash
./build/debug/bin/algovisual
```

For an optimized build use `release` instead of `debug`.

### Build options

| Option              | Default | Description                       |
| ------------------- | ------- | --------------------------------- |
| `AV_BUILD_EXAMPLES` | `ON`    | Build small standalone demos      |
| `AV_BUILD_TESTS`    | `OFF`   | Build unit tests                  |

Pass them at configure time, e.g.:

```bash
cmake --preset debug -DAV_BUILD_TESTS=ON
```

## Development

### VS Code

Open the project folder — recommended extensions are listed in
`.vscode/extensions.json` and VS Code will offer to install them.
Key tools:

- **clangd** for IntelliSense (the bundled `cpptools` IntelliSense is disabled)
- **CMake Tools** for preset management
- **CodeLLDB** for debugging on Linux

The default build task (`Ctrl+Shift+B`) prompts for a preset and runs the
configure + build chain.

### Code style

Formatting is enforced by `clang-format` (config in `.clang-format`,
based on LLVM with 4-space indent and 100-column limit). VS Code is set to
format on save.

### Adding a new scene

1. Create a class deriving from `av::IScene` in `app/src/scenes/`.
2. Implement `name()`, optionally `on_update`, `on_ui` (and `on_render`
   once the 2D renderer lands).
3. Register the scene in `app/src/main.cpp`:

   ```cpp
   app.register_scene(std::make_unique<MyScene>());
   ```

The runtime UI will pick it up automatically.

### Updating ImGui

ImGui is pinned as a submodule. To bump the version:

```bash
cd external/imgui
git fetch --tags
git checkout <new-tag>      # e.g. a docking-branch commit
cd ../..
git add external/imgui
git commit -m "Bump ImGui to <new-tag>"
```

## Roadmap

- [x] Stage 1 — Window + Vulkan instance/device + ImGui
- [ ] Stage 2 — `SceneManager` with runtime scene switching
- [ ] Stage 3 — `Renderer2D` (lines, rectangles, circles) with own pipeline
- [ ] Stage 4 — First real visualization scene (sorting algorithms)
- [ ] Stage 5 — Graph algorithms scene
- [ ] Stage 6 — 2D physics sandbox scene
- [ ] macOS support via MoltenVK

## License

TBD.
