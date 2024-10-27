# Render Engine

School of Computer Engineering and Science, Shanghai University, Computer Graphics Course Project

[中文文档](README.zh.md)

## Introduction

The core code is written in pure C++, using the stdc++20 standard, without relying on any third-party libraries, and can be built directly on any platform to render bmp images directly.
The server is written in C++, depends on the boost and spdlog libraries, provides rendering interfaces, and uses websocket to pass rendering results.

All modules are built using CMake, and the server module build will be canceled if no dependencies are found.

The frontend is written in React, using TypeScript, and built using vite.

## Project Structure

```plaintext
.
├── build
├── docs
└── src
    ├── engine # C++ core engine
    ├── frontend # React frontend
    └── server # C++ server
```

## Render Engine Code Structure

The core logic is located in `src/engine/include/engine.hpp`, which includes primitive storage, frame buffer, and primitive transformation.

Detailed logic can be found in the comments within the code.

`src/engine/include/bitmaps.hpp` contains the class that stores rendered results and can save them as BMP images.

`src/engine/include/primitives.hpp` defines the primitives, including points, lines, triangles, rectangles, etc.

All algorithms are implemented in `src/engine/core/`, divided into different files based on the type of primitive being processed.

`src/engine/test.cpp` is the test file, used to test the engine’s functionality. For each experiment, we tested some basic rendering functions and were able to generate corresponding BMP images.

## Build

1. Clone the repository

```bash
git clone https://github.com/soundofautumn/RenderEngine
```
2. Install dependencies (optional)

The engine core has no dependencies, you can build it directly.
But if you want a real-time GUI, you need to install the dependencies for the server to run.

Server dependencies:

- [boost](https://www.boost.org/)
- [spdlog](https://github.com/gabime/spdlog)

Take vcpkg as an example:

```bash
# Install vcpkg
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
export VCPKG_ROOT="$HOME/vcpkg"
cd "$HOME/vcpkg"
./bootstrap-vcpkg.sh

# You may need to install some dependencies to build vcpkg
brew install autoconf automake autoconf-archive # for macOS

# Install vcpkg packages
vcpkg install boost
vcpkg install spdlog

# Configure CMake toolchain file (you may need to modify the path or the generator)
cp CMakePresets.default.json CMakePresets.json
```

3. Build Core and Server

```bash
# Configure
cmake -S . -B build --preset=default

# Build target files
cmake --build build

# Run the server
cd build && ./RenderEngineServer
```

4. Build Frontend

```bash
cd src/frontend
yarn install # or npm install? is ok
yarn build # or npm run build
```

## API Documentation

[Online Documentation at ApiFox](https://apifox.com/apidoc/shared-8cf19dc6-dfdb-48da-8ac1-0f19a5b58529)
