# Render Engine

上海大学计算机工程与科学学院，计算机图形学课程项目

## 介绍

核心代码使用纯 C++ 编写，stdc++20 标准，不依赖任何第三方库，可以直接在任何平台上构建，可以直接渲染出bmp图片。
服务端使用 C++ 编写，依赖 boost 和 spdlog 库，提供渲染接口，使用 websocket 传递渲染结果。

以上模块均使用 CMake 构建，在未找到依赖时会取消服务端模块的构建。

前端使用 React 编写，使用 TypeScript，使用 vite 构建。

## 项目结构

```plaintext
.
├── build
├── docs
└── src
    ├── engine # C++ 核心引擎
    ├── frontend # React 前端
    └── server # C++ 服务器
```

## 渲染引擎代码结构

核心逻辑在 `src/engine/include/engine.hpp` 中，包含了图元存储、帧缓冲、图元转换等

具体逻辑可以看代码中的注释

`src/engine/include/bitmaps.hpp` 是存储渲染结果的类，可以保存为 bmp 图片

`src/engine/include/primitives.hpp` 是图元的定义，包括点、线、三角形、矩形等

所有算法实现在 `src/engine/core/` 中，根据处理的图元不同，分为不同的文件

`src/engine/test.cpp` 是测试文件，可以用来测试引擎的功能，我们针对每个实验都测试了一些基本绘制功能，并能够生成对应的 bmp 图片

## 构建

1. 克隆仓库

```bash
git clone https://github.com/soundofautumn/RenderEngine
```

2. 安装依赖（可选）

引擎核心没有依赖，你可以直接构建它。
但如果你想要实时GUI，你需要安装服务器运行所需的依赖。

服务器依赖：

- [boost](https://www.boost.org/)
- [spdlog](https://github.com/gabime/spdlog)

以 vcpkg 为例：

```bash
# 安装 vcpkg
git clone https://github.com/microsoft/vcpkg "$HOME/vcpkg"
export VCPKG_ROOT="$HOME/vcpkg"
cd "$HOME/vcpkg"
./bootstrap-vcpkg.sh

# 你可能需要安装一些依赖来构建 vcpkg
brew install autoconf automake autoconf-archive # for macOS

# 安装 vcpkg 包
vcpkg install boost
vcpkg install spdlog

# 配置 CMake 工具链文件（你可能需要修改路径或生成器）
cp CMakePresets.default.json CMakePresets.json
```

3. 构建核心和服务器

```bash
# 配置
cmake -S . -B build --preset=default

# 构建目标文件
cmake --build build

# 运行服务器
cd build && ./RenderEngineServer
```

4. 构建前端
```bash
cd src/frontend
yarn install
yarn build
```

## API 文档

[在线文档](https://apifox.com/apidoc/shared-8cf19dc6-dfdb-48da-8ac1-0f19a5b58529)
