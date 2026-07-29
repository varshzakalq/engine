# Custom C++ Software Renderer — engine

A small, from‑scratch CPU software renderer built in modern C++ and SDL3. This project is a development journal exploring the fundamentals of a 3D rendering pipeline: OBJ model loading, transforms, clipping, perspective projection, z‑buffered triangle rasterization, texture mapping and basic lighting — all implemented without GPU shaders.

## Highlights / What it demonstrates
- Full software rendering pipeline implemented on the CPU.
- Custom Wavefront OBJ loader that packs unique vertex combinations into indexed buffers.
- Model transforms and normal handling (rotation + normalization).
- Near-plane clipping to handle triangles intersecting the view frustum.
- Per-pixel z-buffering for correct visibility.
- Texture mapping with interpolated UVs and a simple lighting model (flat / smooth).
- Real-time display via SDL3 by uploading a CPU pixel buffer into an SDL texture.

## Quick demo / TL;DR
- Windows: open `build.bat` or run the included `main.exe` (ensure `SDL3.dll` sits next to it).
- From source: compile the .cpp files and link against SDL3; run the produced executable which opens a window and streams the CPU pixel buffer to the screen.

## Repository layout (top-level)
- `main.cpp` — application entry: SDL3 init, pixel buffer -> SDL_Texture streaming, main loop
- `engine.h`, `engine.cpp` — Engine class: pixel buffer, z-buffer, core render pipeline (init, put_pixel, update_and_render)
- `objloader.h`, `objloader.cpp` — OBJ parser: reads .obj, packs unique vertex/uv/normal combos, builds vertex & index buffers
- `camera.h`, `camera.cpp` — projection and clipping logic (clipper / onscreen projection helpers)
- `lighting.h`, `lighting.cpp` — lighting computations (flat and smooth shading helpers)
- `basic.h`, `basic.cpp` — utilities and helper drawing primitives (line rasterizer, etc.)
- `texture.h` — texture wrapper and BMP-loading support
- `assets/` — example models & textures (Crate1.obj, cube.obj, Gun.obj, m4a1_s.obj, crate_1.bmp/jpg, gun.mtl, ...)
- `build.bat` — convenience build script for Windows (example g++ compile line)
- `SDL3.dll`, `main.exe`, `output/` — convenience files and example outputs included in repo

## How it works (high-level)
1. main.cpp creates an SDL window & a streaming SDL_Texture, and holds an in-memory uint32_t pixel array that represents the framebuffer.
2. Engine::init receives the pixel array and dimensions and allocates the z-buffer and other internal structures.
3. The OBJ loader parses model files and produces:
   - vertex_buffer: per-vertex position, normal and UV
   - index_buffer: triangle indices referencing packed vertices
4. Per-frame (Engine::update_and_render):
   - Transform each vertex by a model/world matrix (rotation/translation), rotate normals and normalize them.
   - Clip triangles against a near plane (camera::clipper logic) producing either a triangle or a quad (split into two triangles).
   - Project transformed vertices to screen coordinates (camera/projection helpers).
   - For each visible triangle: compute lighting intensity (lighting::flat_shader or smooth shader), sample texture with interpolated UVs and fill pixels while using the z-buffer to resolve visibility (projection.fill_color is invoked from engine.cpp).
5. The pixel array is uploaded to the SDL_Texture (SDL_UpdateTexture) and presented on-screen via SDL_RenderPresent.

## Build & Run (developer)
Requirements:
- C++ compiler supporting C++11 or later (g++, clang, MSVC).
- SDL3 development libraries & runtime (headers + .lib/.dll).
- Windows: the repo includes a `build.bat` showing an example g++ command (adjust paths accordingly).

Example g++ command (adapt include/lib paths for your system):

g++ main.cpp engine.cpp camera.cpp lighting.cpp basic.cpp objloader.cpp -o main.exe -I"<SDL3_include_path>" -L"<SDL3_lib_path>" -lSDL3

Run:
- Ensure `SDL3.dll` is in the same folder as the executable (Windows) or SDL3 is installed/available on your platform.
- Execute `main.exe`. The program opens a window and displays the rendered scene.

Note: `build.bat` contains a Windows-specific example. If you use Linux/macOS, link against your platform's SDL3 library (and remove `.dll` / .exe references).

## Assets & examples
The `assets/` folder contains multiple test models and textures used for demonstrations:
- cube.obj, Crate1.obj, Gun.obj, gun1.obj, m4a1_s.obj
- crate_1.bmp, crate_1.jpg
- .mtl files used by some model variants

If you add your own .obj files, ensure they have positions (v), and optionally normals (vn) and texture coords (vt) — the loader handles missing attributes but behavior will depend on available data.

## Developer notes & pointers
- Entry points:
  - `main.cpp` — sets up SDL3 and the main loop.
  - `Engine::init(...)` — call to initialize engine state and load textures.
  - `Engine::update_and_render()` — core per-frame pipeline.
- OBJ loader:
  - `objloader.cpp` packs vertex/uv/normal combinations into unique vertex_data structures and returns an index buffer for triangles.
- Clipping:
  - `camera::clipper` performs near-plane clipping producing either 3- or 4-vertex polygon loops; `vertex[3]` is used as a working slot/flag.
- Lighting:
  - `lighting::flat_shader(...)` computes face intensity using a computed face normal and a static light position; `smooth_shader` accepts per-vertex normals for interpolated shading.
- Texture mapping:
  - `Texture` class (in `texture.h`) loads BMP and provides pixel access; `projection.fill_color` (referenced from engine.cpp) performs interpolation and texture sampling.

## Known limitations & suggestions
- No frustum culling, no back-face culling optimizations (could be added to skip unseen triangles earlier).
- The current clipping uses a simple near-plane at `z = 10` — make it configurable.
- Single-threaded CPU rasterizer; potential speedups via multi-threading or SIMD.
- Limited texture filtering (nearest/point sampling); add bilinear filtering for smoother results.
- No explicit resource management for large models; consider streaming or memory handling for bigger scenes.

## Contributing
Contributions, bug reports and pull requests are welcome. If you open a PR, please:
- Include a short description of the change and motivation.
- For code changes, follow the existing naming/structuring style and include small, focused commits.

## License
No license file is present in this repository. Add a LICENSE if you want to open-source the project or specify usage/redistribution terms.

## Contact / Attribution
Project by: varshzakalq 
