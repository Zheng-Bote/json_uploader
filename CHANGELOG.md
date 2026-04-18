# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [Changelog](#changelog)
  - [\[1.2.0\] - 2026-04-18](#120---2026-04-18)
    - [Added](#added)
    - [Changed](#changed)
    - [Fixed](#fixed)
  - [\[1.1.0\] - 2026-03-29](#110---2026-03-29)
    - [Added](#added-1)
  - [\[1.0.0\] - 2026-03-28](#100---2026-03-28)
    - [Added](#added-2)
    - [Changed](#changed-1)
    - [Fixed](#fixed-1)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## [1.2.0] - 2026-04-18

### Added

- **Conan v2 Migration**: Switched from manual library management and `FetchContent` to Conan v2 for dependency management.
  - Full support for `libcurl`, `zlib`, `simdjson`, `zstd`, `nlohmann_json`, `spdlog`, `valijson`, and `libsodium`.
  - Configured static linking by default (`shared=False`) for improved portability.
- **New CLI Parameters**:
  - `--version`: Displays the application version and compiler information.
  - `--check-version`: Checks for newer versions on GitHub using `gh-update-checker`.
- **METAJSON_OBJECT Feature**:
  - Added support for `METAJSON_OBJECT` environment variable as a "JSON Header".
  - Implemented `#{key}` placeholder substitution within the `METAJSON_OBJECT` template.
  - Added automatic JSON-compliant escaping for metadata values to ensure stream validity.
  - Integrated `METAJSON_OBJECT` as the first element of the outgoing JSON array.

### Changed

- Refactored `CMakeLists.txt` for seamless integration with Conan v2 toolchains.
- Updated `src/main.cpp` to utilize `rz_config.hpp` for project metadata.
- Improved the `tests` server build system to be compatible with the new Conan-based environment.
- Renamed internal library tracking file to `project_libs.txt` to avoid collisions.

### Fixed

- Fixed a conflict in `file(GENERATE)` output paths between the main project and test subprojects.
- Resolved compilation issues with header-only libraries in a cross-project context.

## [1.1.0] - 2026-03-29

### Added

- Added `API_META` environment variable to control metadata behavior (`none`, `object`, `single`).
  - `none`: No metadata is sent.
  - `object`: Metadata is sent as a nested object (legacy behavior).
  - `single`: Metadata is sent as individual top-level attributes.

## [1.0.0] - 2026-03-28

### Added

- Integrated `dotenv-cpp` for flexible environment variable management.
- Added support for encrypted `.env` files using `libsodium` (XChaCha20-Poly1305).
- Implemented Gzip compression support alongside existing Zstd support for API uploads.
- Added dynamic log level control via `LOG_LEVEL` environment variable.
- Added metadata extraction from environment variables starting with `META_`.
- Implemented `ExternalProject_Add` in CMake for robust `libsodium` integration.
- Added `--encrypted` and `--env-pass-var` CLI parameters.

### Changed

- Refactored `EnvUtil` to support in-memory parsing of decrypted environment data.
- Updated `UploaderSrv` to handle multiple compression formats and merge metadata into JSON payloads.
- Updated default `.env` file path to `data/json_uploader.env`.
- Switched to C++23 standards for core logic.

### Fixed

- Fixed redundant buffer copies in compression stream handling.
- Improved error reporting for network and authentication failures.
