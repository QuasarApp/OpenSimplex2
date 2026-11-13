# AGENTS.md

Base rules for AI coding agents (and developers) working in the
`QuasarApp/OpenSimplex2` repository.

OpenSimplex2 is a C (C99+) implementation of the OpenSimplex2F noise
(2D/3D/4D). This is the QuasarApp community fork of
[KdotJPG/OpenSimplex2](https://github.com/KdotJPG/OpenSimplex2): it keeps
only the C implementation and ships GLSL/HLSL ports.
License: LGPL-3.0 (`LICENSE`).

**Maintainer:** EndrII (Yankovich Andrei), QuasarApp Group DOO.

## Project architecture

- The noise library is a single C translation unit at the repository root:
  `OpenSimplex2F.c` + `OpenSimplex2F.h`. Context based API: create with
  `OpenSimplex2F(seed, &ctx)`, query with `OpenSimplex2F_noise2/3/4...`
  (`double` in `[-1, 1]`), release with `OpenSimplex2F_free(ctx)` /
  `OpenSimplex2F_shutdown()`.
- The root `CMakeLists.txt` creates two targets: `OpenSimplex2Interface`
  (the public INTERFACE target consumers link against) and
  `OpenSimplex2InterfaceC` (the static library from `OpenSimplex2F.c`).
- `submodule/CMake` (git submodule, [QuasarApp/CMake](https://github.com/QuasarApp/CMake))
  is the QuasarApp CMake layer: default build type `Release`, ccache, and
  the `test` target (`initTests`/`addTests` in `tests/CMakeLists.txt`).
  The `test` target deploys the test executable with `cqtdeployer` and then
  runs it, so **`cqtdeployer` must be on `PATH`** to run
  `cmake --build build --target test`.
- `tests/test_OpenSimplex2F.c` builds the `OpenSimplex2tests` executable
  (requires libpng) and writes 512x512 `test2d.png`/`test3d.png`/`test4d.png`.
- `glsl/` and `hlsl/` are standalone shader ports (OpenSimplex2F +
  OpenSimplex2S), not part of the C build.
- Documentation site: MkDocs + Doxygen (`mkdocs.yml`, `doxygen.conf.in`,
  `res/`), deployed to GitHub Pages on pushes to `main`
  (`.github/workflows/deploy-docs.yml`). The root `README.md` is the single
  source of truth for the site home page.

## Project structure

| Path | Contents |
|---|---|
| `OpenSimplex2F.c`, `OpenSimplex2F.h` | The C implementation and the public API |
| `CMakeLists.txt`, `Makefile` | The CMake build; the manual build (`make`) |
| `tests/` | The `OpenSimplex2tests` executable |
| `glsl/`, `hlsl/` | Standalone shader ports |
| `submodule/CMake` | The QuasarApp CMake layer (git submodule) |
| `.github/workflows/` | `ci.yml` (build + tests), `deploy-docs.yml` (documentation site) |
| `mkdocs.yml`, `doxygen.conf.in`, `res/` | Documentation site configuration |
| `README.md`, `CHANGELOG.md`, `LICENSE` | Description, changelog (Keep a Changelog), LGPL-3.0 |

## Contribution process

Work is issue-driven: one GitHub issue → one pull request.

1. Branch `task_<issue>`, created **strictly from `main`**:
   `git checkout -b task_<issue> origin/main`.
2. Issue labels: `in progress` while working → `waiting-for-review` when the
   PR is ready.
3. Checklist comment on the issue: post it once and update **that same
   comment in place**
   (`gh issue comment <issue> --edit-last --body-file /tmp/checklist.md`).
4. Commits: short imperative subject + issue reference, e.g.
   `docs: add AGENTS.md (issue #32)`.
5. PR via `gh pr create --base main --head task_<issue>`; the title carries
   the issue reference, the body lists what changed and the verification
   actually performed (commands + real output).
6. CI must be green: `ci.yml` builds with CMake + Ninja and runs the `test`
   target on Linux and Windows for every PR.

Rules: C-only (other languages only as thin wrappers around the C API via
PR); do not re-introduce the removed implementations (Rust, C#, Java, C++);
new tests ship **with** the code they pin; user-visible changes go into
`CHANGELOG.md`; keep the LGPL-3.0 headers.

## Building

Prerequisites: CMake ≥ 3.19; GCC or Clang (on Windows a MinGW toolchain —
the test source uses the GCC-only `__attribute__((unused))`); Ninja
(recommended); libpng development headers (the tests); `cqtdeployer` on
`PATH` (the `test` target).

```sh
git clone --recurse-submodules https://github.com/QuasarApp/OpenSimplex2.git
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
cmake --build build --target test   # deploys with cqtdeployer, runs OpenSimplex2tests
```

Options: `OPENSIMPLEX2_TESTS` (default `ON`), `CCACHE_ENABLED` (default `ON`).

Manual build without CMake: `make` (needs libpng) or
`cc -Wall -Wextra --pedantic -O3 -c OpenSimplex2F.c`.

Consumer side:

```cmake
add_subdirectory(third_party/OpenSimplex2)
target_link_libraries(consumer PRIVATE OpenSimplex2Interface)
```

## Testing

- CMake: `cmake --build build --target test` (deploy + run, needs
  `cqtdeployer`).
- Directly: `./build/tests/OpenSimplex2tests` (plain build) or
  `./Testing/OpenSimplex2/Application/bin/OpenSimplex2tests` (deployed by the
  `test` target).
- Makefile: `make && ./test_OpenSimplex2F`.
