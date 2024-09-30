# Render Engine

上海大学计算机工程与科学学院 《计算机图形学》课程项目

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

## Build

1. Clone the repository

```bash
git clone https://github.com/soundofautumn/RenderEngine
```
2. Install dependencies (optional)

The engine core has no dependencies, you can build it directly.
But if you want a real-time visualization, you need to install the dependencies for the server to run.

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
# Create build directory
mkdir build
cd build

# Configure CMake
cmake --preset=default ..

# Build target files
cmake --build .

# Run the server
./RenderEngineServer
```

4. Build Frontend

```bash
cd src/frontend
yarn install # or npm install? is ok
yarn build # or npm run build
```

## API Documentation

[Online Documentation at ApiFox](https://apifox.com/apidoc/shared-8cf19dc6-dfdb-48da-8ac1-0f19a5b58529)
