# Changelog

All notable changes to cppseed are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

No user-visible changes have been added since v0.1.0.

## [0.1.0] - 2026-07-14

### Added

- Generate a buildable CMake project with `cppseed new <project-name>`.
- Select C++17, C++20, or C++23 with the `--std` option.
- Generate a library, executable, dependency-free CTest test, `.clang-format`,
  `.gitignore`, and project README.
- Protect existing paths and remove partial output after generation failures.
- Provide dependency-free command-line binaries for macOS Apple Silicon,
  macOS Intel, Linux x86-64, and Windows x86-64.
- Publish SHA-256 checksums and run native release smoke tests for every binary.

[Unreleased]: https://github.com/harutasti/cppseed/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/harutasti/cppseed/releases/tag/v0.1.0
