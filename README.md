# cppseed

`cppseed` is a dependency-free C++ CLI for generating small, buildable CMake
projects.

> Status: v0.1.0 implementation in progress

Design documents:

- [v0.1 requirements](docs/requirements.md)
- [v0.1 external design](docs/basic-design.md)
- [v0.1 internal design](docs/detailed-design.md)
- [v0.1 test plan](docs/test-plan.md)

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

## Build from source

Building cppseed itself requires a C++20 compiler and CMake 3.20 or newer.

```console
cmake -S . -B build -DCPPSEED_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The CI matrix covers GCC and Clang on Linux, Apple Clang on Apple Silicon and
Intel macOS, and MSVC on Windows.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

cppseed is available under the [MIT License](LICENSE). Generated project code
is not restricted by cppseed's license.
