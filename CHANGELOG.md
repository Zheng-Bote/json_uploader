<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Changelog](#changelog)
  - [[1.0.0] - 2026-03-28](#100---2026-03-28)
    - [Added](#added)
    - [Changed](#changed)
    - [Fixed](#fixed)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
