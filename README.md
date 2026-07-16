# cppseed

`cppseed` is a dependency-free C++ CLI for generating small, buildable CMake
projects.

> Latest stable release: [v0.1.0](https://github.com/harutasti/cppseed/releases/tag/v0.1.0)

See [CHANGELOG.md](CHANGELOG.md) for released and upcoming changes.

v0.1.0 design documents:

- [v0.1 requirements](docs/requirements.md)
- [v0.1 external design](docs/basic-design.md)
- [v0.1 internal design](docs/detailed-design.md)
- [v0.1 test plan](docs/test-plan.md)
- [v0.1 release plan](docs/release-plan.md)

Post-v0.1 release engineering:

- [SBOM and build provenance requirements](docs/supply-chain-requirements.md)

## Usage

```console
cppseed new my-project
cppseed new my-project --std 23
```

The generated project contains a small library, executable, and dependency-free
CTest test. It supports C++17, C++20 (the default), and C++23.

```console
cd my-project
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Running a released cppseed binary does not require a compiler, CMake, Python,
Rust, Node.js, or another language runtime. A compiler and CMake are needed only
to build the generated C++ project.

## Install a binary release

Download the archive for your system from
[GitHub Releases](https://github.com/harutasti/cppseed/releases):

| System | Archive suffix |
|---|---|
| macOS Apple Silicon | `aarch64-apple-darwin.tar.gz` |
| macOS Intel | `x86_64-apple-darwin.tar.gz` |
| Linux x86-64 | `x86_64-unknown-linux-gnu.tar.gz` |
| Windows x86-64 | `x86_64-pc-windows-msvc.zip` |

Each archive contains the executable, `LICENSE`, and `README.md`. Extract it and
place `cppseed` (or `cppseed.exe`) in a directory on your `PATH`.

Verify SHA-256 before running the binary:

```console
# Linux
sha256sum -c SHA256SUMS --ignore-missing

# macOS
shasum -a 256 cppseed-v0.1.0-aarch64-apple-darwin.tar.gz

# Windows PowerShell
Get-FileHash .\cppseed-v0.1.0-x86_64-pc-windows-msvc.zip -Algorithm SHA256
```

Compare macOS and Windows output with the matching line in `SHA256SUMS`, then
confirm the installed version:

```console
cppseed --version
```

The v0.1.0 binaries are unsigned. macOS Gatekeeper or Windows SmartScreen may
show a warning; verify the checksum and GitHub repository origin before
proceeding. The Linux x86-64 binary targets glibc 2.35 or newer, and the macOS
binaries target macOS 13 or newer.

## Build from source

Building cppseed itself requires a C++20 compiler and CMake 3.20 or newer.

```console
cmake -S . -B build -DCPPSEED_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The CI matrix covers GCC and Clang on Linux, Apple Clang on Apple Silicon and
Intel macOS, and MSVC on Windows.

The source compatibility baseline is GCC 11, Clang 14, Apple Clang 14, or MSVC
19.34 (Visual Studio 2022 17.4). The exact compiler and runner versions tested
for a release are recorded in its GitHub Actions run.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

Security vulnerabilities should be reported according to
[SECURITY.md](SECURITY.md), not in a public issue.

## License

cppseed is available under the [MIT License](LICENSE). Generated project code
is not restricted by cppseed's license.
