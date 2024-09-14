# Build

```bash
# Install vcpkg for C and C++ dependencies
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
export VCPKG_ROOT="$HOME/vcpkg"
# Install some fucking softwares for boost to install python3
brew install autoconf automake autoconf-archive # for macOS
# Install vcpkg packages
vcpkg install boost
vcpkg install spdlog
# Build target files
cmake --preset=default
cmake --build build
# Add soft link to target executable files
ln -s build/src/engine/RenderEngineCoreTest ./RenderEngineCoreTest
ln -s build/src/server/RenderEngineServer ./RenderEngineServer
# Start
./RenderEngineServer
```
