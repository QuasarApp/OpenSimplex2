# OpenSimplex 2

Successors to OpenSimplex Noise, plus updated OpenSimplex. Includes 2D, 3D, and 4D noise.

## About this fork

This is the QuasarApp community fork of
[KdotJPG/OpenSimplex2](https://github.com/KdotJPG/OpenSimplex2).

- **Performance oriented — C only.** The OpenSimplex2 method is performance
  oriented, so this fork allows only the C implementation — because it is the
  most powerful. The C implementation is the single original, most fast and
  safe implementation of the noise.
- **The Rust implementation was removed.** The Rust implementation was still
  slow and used a 32 bit source, so it may break high quality results.
- **Why a fork?** The maintainers of the original repository are not active
  and do not fix currently critical bugs.
- **Other languages?** If you need an implementation in another language, just
  create a simple wrapper around the C API and create a Merge Request.

## Motivation

Legacy OpenSimplex used a different grid layout scheme to Simplex. It worked fine for common applications, but didn't produce as consistent contrast as the original Simplex point layout in 3D/4D. It also broke down completely when extended to 5D+, producing diagonal bands of visibly-varying value ranges.

Instead of vertex rearrangement, OpenSimplex2 focuses on congruent layouts constructed and traversed using disparate methods where need was concluded.

Gradient vector tables were also revisited to improve probability symmetry in both the old and new noise.

## Included

### OpenSimplex2(F)
 * Looks the most like Simplex.
 * Is about as fast as common Simplex implementations.
 * 3D is implemented by constructing a rotated body-centered-cubic grid as the offset union of two rotated cubic grids, and finding the closest two points on each.
 * 4D is implemented using 5 offset copies of the dual (reverse-skewed) grid to produce the target grid. This technique generalizes to any number of dimensions.
 * The bundled 2D function is just 2D Simplex with a nice gradient table.
 * The C implementation provides the OpenSimplex2(F) noise in 2D, 3D and 4D.

### Shaders

The `glsl/` and `hlsl/` directories provide GLSL and HLSL shader versions of
OpenSimplex2(F) and OpenSimplex2S. OpenSimplex2S looks the most like 2014
OpenSimplex: it uses large vertex contribution ranges, but has better
uniformity in 3D and 4D, and is the recommended choice for ridged noise.

## Usage

The source (`OpenSimplex2F.c`) and the public header (`OpenSimplex2F.h`)
are located at the root of this repository.

### C

```c
#include <OpenSimplex2F.h>

/* Allocate and initialize the noise context (0 on success). */
struct OpenSimplex2F_context *ctx = NULL;
if (OpenSimplex2F(42, &ctx) != 0) {
    /* context creation failed */
    return 1;
}

/* 2D, 3D and 4D noise, each in several lattice orientations. */
double n2 = OpenSimplex2F_noise2(ctx, 0.5, 0.5);
double n3 = OpenSimplex2F_noise3_XYBeforeZ(ctx, 0.5, 0.5, 0.5);
double n4 = OpenSimplex2F_noise4_XYBeforeZW(ctx, 0.5, 0.5, 0.5, 0.5);

/* Release the context and the singleton lattice point data. */
OpenSimplex2F_free(ctx);
OpenSimplex2F_shutdown();
```

### C++

The header is C-compatible (`extern "C"` guarded), so the same API can be
used directly from C++:

```cpp
#include <OpenSimplex2F.h>

struct OpenSimplex2F_context *ctx = nullptr;
if (OpenSimplex2F(42, &ctx) == 0) {
    double value = OpenSimplex2F_noise3_XYBeforeZ(ctx, x, y, z);
    /* ... */
    OpenSimplex2F_free(ctx);
}
OpenSimplex2F_shutdown();
```

### CMake

To include the library in your own CMake project, add the repository as a
subdirectory — for example as a git submodule:

```
git submodule add https://github.com/QuasarApp/OpenSimplex2.git third_party/OpenSimplex2
```

and link the `OpenSimplex2Interface` target in your `CMakeLists.txt`:

```cmake
# Consumer project CMakeLists.txt
cmake_minimum_required(VERSION 3.19)
project(consumer LANGUAGES C)

add_subdirectory(third_party/OpenSimplex2)

add_executable(consumer main.c)

# OpenSimplex2Interface is an INTERFACE target: it links the C
# implementation (OpenSimplex2F.c), exposes the repository root as a
# public include directory and links the C math library (-lm).
target_link_libraries(consumer PRIVATE OpenSimplex2Interface)
```

### Manual build

Compile the source file into your project with your C compiler, for example:

```
cc -Wall -Wextra --pedantic -O3 -c OpenSimplex2F.c -lm
```

Or use the bundled Makefile:

```
make
```

The Makefile also builds the `test_OpenSimplex2F` binary (test source in
`tests/test_OpenSimplex2F.c`), which writes 2D/3D/4D noise images
(requires libpng).

### Tests

To build the tests, keep the `OPENSIMPLEX2_TESTS` option enabled (default
`ON`) and run the `test` target — it builds the `OpenSimplex2tests`
executable from the `tests/` subdirectory and runs it (the targets are
registered with the `addTests()` function of the QuasarApp CMake module):

```
cmake -B build
cmake --build build
cmake --build build --target test
```

## License

This project is provided under the license defined in the
[LICENSE](LICENSE) file.
