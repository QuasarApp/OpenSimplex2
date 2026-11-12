# AGENTS.md

Base rules for AI coding agents (and developers) working in the
`QuasarApp/OpenSimplex2` repository.

## Description

OpenSimplex 2 is a performance-oriented **C** implementation of the
OpenSimplex2(F) noise in 2D, 3D and 4D. This repository is the QuasarApp
community fork of [KdotJPG/OpenSimplex2](https://github.com/KdotJPG/OpenSimplex2)
(the upstream maintainers are inactive; the fork keeps only the C
implementation — it is the fastest and the original one — and ships GLSL/HLSL
shader ports as well).

- **Language:** C (C99+; the public header is C++ compatible via an
  `extern "C"` guard)
- **Build system:** CMake ≥ 3.19 (Ninja recommended)
- **License:** GNU Lesser General Public License v3.0 (LGPL-3.0), see
  `LICENSE`. `hlsl/UNLICENSE` is kept for the third-party-derived HLSL code.

### Maintainer

**EndrII (Yankovich Andrei)** — team lead, QuasarApp Group DOO.
All contributions are reviewed by the QuasarApp team; the project is
maintained in the `QuasarApp` GitHub organization.

## Project architecture

- **The noise library is a single C translation unit.**
  `OpenSimplex2F.c` (implementation) + `OpenSimplex2F.h` (public API) live at
  the repository root. The API is context based:

  ```c
  int OpenSimplex2F(int64_t seed, struct OpenSimplex2F_context **ctx); // create
  double OpenSimplex2F_noise2(ctx, x, y);                              // + variants
  double OpenSimplex2F_noise3_Classic / _XYBeforeZ / _XZBeforeY(ctx, x, y, z);
  double OpenSimplex2F_noise4_Classic / _XYBeforeZW / _XZBeforeYW / _XYZBeforeW(...);
  void OpenSimplex2F_free(ctx);   // release the context
  void OpenSimplex2F_shutdown();  // release the singleton lattice point data
  ```

  2D is standard simplex with a gradient table; 3D is the re-oriented
  4-point BCC lattice; 4D builds the lattice as the union of five copies of
  its reciprocal. Values are `double` in `[-1, 1]`.
- **CMake packaging.** The top-level `CMakeLists.txt` creates two targets:
  - `OpenSimplex2Interface` — the **public INTERFACE target**: consumers link
    against it; it exposes the repository root as the public include
    directory and links the C math library (`m`).
  - `OpenSimplex2InterfaceC` — the static library built from
    `OpenSimplex2F.c` with `-Wall -Wextra --pedantic`, linked into the
    interface target.
- **QuasarApp CMake layer.** `include(submodule/CMake/QuasarApp.cmake)`
  (submodule `submodule/CMake` → [QuasarApp/CMake](https://github.com/QuasarApp/CMake))
  sets the default build type (`Release`), enables ccache and registers the
  CI targets. `initAll()` in the root file and `initTests()`/`addTests()` in
  `tests/CMakeLists.txt` register the `test` target: it deploys the test
  executable with `cqtdeployer` and then executes it, so **`cqtdeployer` must
  be on `PATH`** to run `cmake --build build --target test`.
- **Tests.** `tests/test_OpenSimplex2F.c` builds the `OpenSimplex2tests`
  executable (requires `find_package(PNG REQUIRED)`, i.e. libpng). At runtime
  it generates 512×512 noise images `test2d.png`, `test3d.png`, `test4d.png`
  for the 2D/3D/4D functions.
- **Shader ports (no build system).** `glsl/OpenSimplex2.glsl`,
  `glsl/OpenSimplex2S.glsl`, `hlsl/OpenSimplex2.hlsl`, `hlsl/OpenSimplex2S.hlsl`
  are standalone GLSL/HLSL ports of OpenSimplex2F and OpenSimplex2S for
  shader consumers; they are not part of the C build. OpenSimplex2S is the
  variant recommended for ridged noise.
- **Documentation site.** MkDocs + Material theme (teal palette, `mkdocs.yml`)
  with Doxygen API docs served under `/api/` (`doxygen.conf.in`,
  `res/doxygen-material.css`). `.github/workflows/deploy-docs.yml` rebuilds
  the site on every push to `main` and deploys it to the `gh-pages` branch
  (<https://quasarapp.github.io/OpenSimplex2/>). The root `README.md` is the
  single source of truth for the site home page: the workflow copies it to
  `docs/index.md` at build time — never commit `docs/index.md` or `site/`.

## Project structure

| Path | Contents |
|---|---|
| `OpenSimplex2F.c` | The C implementation of the OpenSimplex2F noise (2D/3D/4D) |
| `OpenSimplex2F.h` | Public C API (context create/free, all noise functions) |
| `CMakeLists.txt` | Root CMake: `OpenSimplex2Interface` + `OpenSimplex2InterfaceC` targets, `OPENSIMPLEX2_TESTS` option |
| `Makefile` | Manual build: `make` builds `OpenSimplex2F.o` and the `test_OpenSimplex2F` binary (needs libpng) |
| `tests/CMakeLists.txt`, `tests/test_OpenSimplex2F.c` | The `OpenSimplex2tests` executable + its CMake registration (`initTests`/`addTests`) |
| `glsl/`, `hlsl/` | Standalone GLSL/HLSL shader ports (OpenSimplex2F + OpenSimplex2S) |
| `submodule/CMake` | Git submodule [QuasarApp/CMake](https://github.com/QuasarApp/CMake): the QuasarApp CMake layer (`QuasarApp.cmake`, ccache, CI targets, Qt iOS/Android helpers) |
| `.github/workflows/ci.yml` | CI: build + `test` target on ubuntu-latest and windows-latest for every PR |
| `.github/workflows/deploy-docs.yml` | Documentation site build + GitHub Pages deployment on pushes to `main` |
| `mkdocs.yml`, `doxygen.conf.in`, `res/doxygen-material.css` | Documentation site configuration |
| `README.md` | Project description, usage examples (C, C++, CMake, manual build) |
| `CHANGELOG.md` | Keep a Changelog formatted release notes |
| `LICENSE` | LGPL-3.0 |

## Contribution process (workflow for contributors)

Work is **issue-driven**: one GitHub issue → one pull request.

1. **Branching.** Create one branch named `task_<issue-number>` branched
   **strictly from `main`** — never from another task branch:
   ```sh
   git fetch origin
   git checkout -b task_<issue> origin/main
   ```
2. **Labels.** Set the `in progress` label on the issue while working (the
   label is spelled with a space in this repository), and move it to
   `waiting-for-review` when the PR is ready:
   ```sh
   gh issue edit <issue> --add-label "in progress"
   gh issue edit <issue> --remove-label "in progress" --add-label waiting-for-review
   ```
   Other useful labels: `documentation`, `blocked`, `help wanted`,
   `high priority`.
3. **Checklist.** Post the initial checklist as a comment on the issue and
   update **that same comment in place** after each step
   (`gh issue comment <issue> --edit-last --body-file /tmp/checklist.md`) —
   never re-post it.
4. **Commits.** Short imperative subject + issue reference, e.g.
   `docs: add AGENTS.md with architecture and build instructions (issue #32)`.
5. **Pull request.** Create the PR with the `gh` CLI against `main`; the
   title carries the issue reference, the body lists what changed and the
   verification actually performed (commands + real output):
   ```sh
   git push -u origin task_<issue>
   gh pr create --base main --head task_<issue> \
     --title "<type>: <short imperative summary> (issue #<issue>)" \
     --body-file /tmp/pr_body.md
   ```
   A PR exists only when `gh pr create` prints its URL — confirm with
   `gh pr view <number> --json number,url,mergeable` before reporting it.
6. **CI.** CI (`.github/workflows/ci.yml`) builds the project with CMake +
   Ninja and runs the `test` target on Linux and Windows for every PR. Keep
   the build and tests green — a red CI blocks the merge.

### Contribution rules

- Keep the scope on C: this fork is C-only. Other languages are added as
  thin wrappers around the C API via pull request.
- Do not re-introduce the removed implementations (Rust, C#, Java, C++).
- New tests ship **with** the code they pin — never in a follow-up ticket.
- Update `CHANGELOG.md` (Keep a Changelog format) for user-visible changes.
- Respect the license: new code is LGPL-3.0, keep the copyright headers.

## Building the project

### Prerequisites

| Tool | Notes |
|---|---|
| CMake | ≥ 3.19 |
| C compiler | GCC or Clang. On Windows a MinGW toolchain is required — the test source uses GCC-only `__attribute__((unused))`, MSVC cannot build it |
| Ninja | recommended generator |
| libpng | development headers (`libpng-dev` on Linux, `mingw-w64-x86_64-libpng` via MSYS2 on Windows) — required by the test executable |
| cqtdeployer | `cqtdeployer` on `PATH` — required by the CMake `test` target (it deploys the test executable before running it) |

### Step by step (the same commands CI runs)

```sh
# 1. get the sources (submodule/CMake is mandatory — it provides the
#    QuasarApp CMake layer used by the root CMakeLists.txt)
git clone --recurse-submodules https://github.com/QuasarApp/OpenSimplex2.git
# or, for an existing clone:
git submodule update --init --recursive

# 2. configure (Ninja recommended; default build type is Release — the
#    QuasarApp CMake module sets it when CMAKE_BUILD_TYPE is not given)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 3. build
cmake --build build --parallel 4

# 4. run the tests (deploys the test executable with cqtdeployer, then runs
#    OpenSimplex2tests, which writes test2d.png / test3d.png / test4d.png)
cmake --build build --target test
```

CMake options (top level):

- `OPENSIMPLEX2_TESTS` — build the tests (`tests/` subdirectory), default `ON`
- `CCACHE_ENABLED` — use ccache, default `ON` (QuasarApp CMake layer)

### Manual build (no CMake)

The bundled `Makefile` builds `OpenSimplex2F.o` and the `test_OpenSimplex2F`
binary (needs libpng):

```sh
make          # -> OpenSimplex2F.o + test_OpenSimplex2F
make clean
```

Or compile the single source file directly:

```sh
cc -Wall -Wextra --pedantic -O3 -c OpenSimplex2F.c
```

### Using the library from your own CMake project

Add the repository as a subdirectory (e.g. git submodule) and link the
interface target:

```cmake
add_subdirectory(third_party/OpenSimplex2)
add_executable(consumer main.c)
target_link_libraries(consumer PRIVATE OpenSimplex2Interface)
```

## Testing the project

- **Test executable:** `OpenSimplex2tests` (source: `tests/test_OpenSimplex2F.c`).
  It exercises the 2D/3D/4D noise functions and writes the 512×512 images
  `test2d.png`, `test3d.png`, `test4d.png` (libpng). Built when
  `OPENSIMPLEX2_TESTS=ON` (default).
- **Run the tests from CMake** — the `test` target created by the QuasarApp
  CMake layer (`initTests`/`addTests`) deploys the test executable with
  `cqtdeployer` and then executes it:
  ```sh
  cmake -B build -G Ninja
  cmake --build build
  cmake --build build --target test
  ```
  Requirements: libpng for the build, `cqtdeployer` on `PATH` for the target.
- **Run the test binary directly** (after a plain build):
  ```sh
  ./build/tests/OpenSimplex2tests   # plain build location
  ./Testing/OpenSimplex2/Application/bin/OpenSimplex2tests  # deployed by the `test` target
  # or the Makefile build:
  make && ./test_OpenSimplex2F
  ```
- **CI:** `.github/workflows/ci.yml` runs on every PR (opened/synchronize/
  reopened) on `ubuntu-latest` and `windows-latest` (Release, Ninja, ccache).
  Windows uses the aqt MinGW toolchain (`tools_mingw1310`) plus MSYS2 libpng
  with `mingw64/bin` added to `PATH` so the test executable finds
  `libpng16-16.dll` at runtime. The documentation workflow
  (`deploy-docs.yml`) runs separately on pushes to `main`.

