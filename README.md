# Noxel Engine

**Status: Active Early Development**

Noxel Engine is a modern C++23 game engine.

![Engine Screenshot](docs/images/overview.png)

## Getting Started

### Requirements

- A C++23 compatible compiler (GCC 12+, Clang 15+, or MSVC 19.34+)
- [CMake](https://cmake.org/download/) (v3.5 or higher)
- [vcpkg](https://vcpkg.io/en/getting-started.html) for managing dependencies

### Building

1. **Clone the repository:**
   ```bash
   git clone https://github.com/l-nikita/noxel-engine.git
   cd noxel-engine
   ```

2. **Configure with vcpkg and CMake:**
   ```bash
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```

3. **Build the project:**
   ```bash
   cmake --build build
   ```
