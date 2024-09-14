//
// Created by Autumn Sound on 2024/9/5.
//

//=====================================================================
// 位图库：Bitmap
// 说明：提供一个简单的位图类，支持加载和保存BMP文件
// 修改自：https://github.com/skywind3000/RenderHelp/blob/master/RenderHelp.h
//=====================================================================

#ifndef RENDERENGINE_BITMAP_HPP
#define RENDERENGINE_BITMAP_HPP

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>
#include <fstream>
#include <vector>

namespace RenderCore {
    class Bitmap;
}

class RenderCore::Bitmap {
protected:
    // 宽度
    int32_t width_;
    // 高度
    int32_t height_;
    // 每行字节数
    int32_t pitch_;
    // 数据
    uint8_t *data_;

public:
    inline Bitmap(int32_t width, int32_t height) :
            width_(width),
            height_(height),
            pitch_(width * 4),
            data_(new uint8_t[width * height * 4]) {}

    inline Bitmap(const Bitmap &bitmap) :
            width_(bitmap.width_),
            height_(bitmap.height_),
            pitch_(bitmap.pitch_),
            data_(new uint8_t[bitmap.width_ * bitmap.height_ * 4]) {
        std::memcpy(data_, bitmap.data_, width_ * height_ * 4);
    }

    inline ~Bitmap() {
        delete[] data_;
    }

public:

    [[nodiscard]] inline int width() const {
        return width_;
    }

    [[nodiscard]] inline int height() const {
        return height_;
    }

    [[nodiscard]] inline int pitch() const {
        return pitch_;
    }

    [[nodiscard]] inline uint8_t *data() const {
        return data_;
    }

    [[nodiscard]] inline const uint8_t *data() {
        return data_;
    }

    [[nodiscard]] inline uint8_t *line(int32_t y) {
        return data_ + y * pitch_;
    }

    [[nodiscard]] inline const uint8_t *line(int32_t y) const {
        return data_ + y * pitch_;
    }

public:
    inline void fill(uint32_t color) {
        for (int i = 0; i < height_; i++) {
            auto *row = reinterpret_cast<uint32_t *>(data_ + i * pitch_);
            for (int j = 0; j < width_; j++) {
                row[j] = color;
            }
        }
    }

    inline void set_pixel(int32_t x, int32_t y, uint32_t color) {
        assert(x >= 0 && x < width_ && y >= 0 && y < height_);
        auto *pixel = reinterpret_cast<uint32_t *>(data_ + y * pitch_ + x * 4);
        *pixel = color;
    }

    [[nodiscard]] inline uint32_t get_pixel(int32_t x, int32_t y) const {
        assert(x >= 0 && x < width_ && y >= 0 && y < height_);
        auto *pixel = reinterpret_cast<const uint32_t *>(data_ + y * pitch_ + x * 4);
        return *pixel;
    }

private:
    struct BitmapInfoHeader {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t planes;
        uint16_t bit_count;
        uint32_t compression;
        uint32_t size_image;
        int32_t x_pels_per_meter;
        int32_t y_pels_per_meter;
        uint32_t clr_used;
        uint32_t clr_important;
    };
public:
    using Buffer = std::vector<uint8_t>;

    inline static std::shared_ptr<Bitmap> load_bmp(const char *filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            return nullptr;
        }
        char header[2];
        file.read(header, 2);
        if (header[0] != 'B' || header[1] != 'M') {
            return nullptr;
        }
        uint32_t file_size;
        file.read(reinterpret_cast<char *>(&file_size), 4);
        file.seekg(4, std::ios::cur);
        uint32_t offset;
        file.read(reinterpret_cast<char *>(&offset), 4);
        BitmapInfoHeader info_header{};
        file.read(reinterpret_cast<char *>(&info_header), sizeof(BitmapInfoHeader));
        if (info_header.bit_count != 24 && info_header.bit_count != 32) {
            return nullptr;
        }
        const int32_t width = info_header.width;
        const int32_t height = info_header.height;
        const int32_t pitch = (width * 4 + 3) & (~3);
        auto bitmap = std::make_shared<Bitmap>(width, height);
        for (int i = 0; i < height; i++) {
            uint8_t *line = bitmap->line(height - i - 1);
            file.read(reinterpret_cast<char *>(line), pitch);
        }
        return bitmap;
    }

    [[nodiscard]] inline Buffer save_to_buffer() const {
        return Buffer{data_, data_ + width_ * height_ * 4};
    }

    inline bool save_bmp(const char *filename, bool with_alpha = false) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        const uint32_t pixel_size = with_alpha ? 4 : 3;
        const uint32_t pitch = (width_ * pixel_size + 3) & (~3);
        const uint32_t file_size = 14 + sizeof(BitmapInfoHeader) + pitch * height_;
        const uint32_t offset = 14 + sizeof(BitmapInfoHeader);
        BitmapInfoHeader info_header{
                sizeof(BitmapInfoHeader),
                width_,
                height_,
                1,
                static_cast<uint16_t>(pixel_size * 8),
                0,
                pitch * height_,
                0,
                0,
                0,
                0
        };
        file.write("BM", 2);
        file.write(reinterpret_cast<const char *>(&file_size), 4);
        file.write("\0\0\0\0", 4);
        file.write(reinterpret_cast<const char *>(&offset), 4);
        file.write(reinterpret_cast<const char *>(&info_header), sizeof(BitmapInfoHeader));
        for (int i = 0; i < height_; i++) {
            // bmp文件是从下到上保存的
            const uint8_t *line = this->line(height_ - i - 1);
            const uint32_t padding = pitch - width_ * pixel_size;
            for (int j = 0; j < width_; j++) {
                // bmp文件是BGRA顺序
                const uint8_t r = line[3];
                const uint8_t g = line[2];
                const uint8_t b = line[1];
                const uint8_t a = with_alpha ? line[0] : 0xFF;
                const uint32_t pixel = (r << 16) | (g << 8) | b | (a << 24);
                file.write(reinterpret_cast<const char *>(&pixel), pixel_size);
                line += 4;
            }
            for (uint32_t j = 0; j < padding; j++) {
                file.put(0);
            }
        }
        return true;
    }


};

#endif //RENDERENGINE_BITMAP_HPP
