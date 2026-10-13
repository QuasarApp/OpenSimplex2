# Changelog

All notable changes to the QuasarApp OpenSimplex2 fork are documented in
this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

## [1.0.0] - 2026-10-13

First "Comunity Release" (milestone [#1](https://github.com/QuasarApp/OpenSimplex2/milestone/1))
of the QuasarApp community fork of
[KdotJPG/OpenSimplex2](https://github.com/KdotJPG/OpenSimplex2).

### Added

- GitHub Actions CI (`.github/workflows/ci.yml`): builds the project with
  CMake + Ninja and runs the `test` target (deployed via CQtDeployer) on
  Linux and Windows for every pull request
  ([#7](https://github.com/QuasarApp/OpenSimplex2/issues/7)).
- GitHub Pages documentation site
  ([#12](https://github.com/QuasarApp/OpenSimplex2/issues/12)):
  - MkDocs with the Material theme and a teal brand palette
    (`mkdocs.yml`, `docs/overrides/main.css`),
  - Doxygen API documentation served under `/api/`
    (`doxygen.conf.in`, `res/doxygen-material.css`),
  - `deploy-docs.yml` workflow that rebuilds the site on every push to
    `main` and deploys it to the `gh-pages` branch.
- This `CHANGELOG.md`
  ([#13](https://github.com/QuasarApp/OpenSimplex2/issues/13)).

### Changed

- `README.md` rewritten for the fork
  ([#8](https://github.com/QuasarApp/OpenSimplex2/issues/8)):
  - fork main points: performance oriented, C implementation only, Rust
    implementation removed (it was slow and used a 32 bit source),
    upstream maintainers inactive, wrappers for other languages via
    Merge Request,
  - usage examples in C, C++ and CMake (consumer `CMakeLists.txt` using
    `add_subdirectory()` and the `OpenSimplex2Interface` target), manual
    build and test target usage,
  - new License section pointing to the `LICENSE` file,
  - upstream-only content removed (2020-era changelog entries, renders of
    noise variants not part of the C build, area-generator note).
- License changed from CC0 1.0 Universal to the GNU Lesser General Public
  License v3.0 (LGPL-3.0) with
  `Copyright (C) 2026 QuasarApp Group DOO`
  ([#9](https://github.com/QuasarApp/OpenSimplex2/issues/9)). All code
  added to the repository after this release is licensed under LGPLv3;
  existing upstream-derived sources are unchanged.

### Removed

- The non-C implementations (Rust, C#, Java, C++) — this fork keeps only
  the C implementation; done before this release, announced in the
  rewritten `README.md`.
- The root `UNLICENSE` file, which contradicted the new LGPLv3 license
  ([#9](https://github.com/QuasarApp/OpenSimplex2/issues/9)). Note:
  `hlsl/UNLICENSE` is kept — it marks the existing third-party-derived
  HLSL code.
