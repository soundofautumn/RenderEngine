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

class Bitmap {
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

    inline static std::shared_ptr<Bitmap> load_from_buffer(const Buffer &buffer) {
        if (buffer.size() < 14 + sizeof(BitmapInfoHeader)) {
            return nullptr;
        }
        const uint8_t *data = buffer.data();
        const uint8_t *header = data;
        if (header[0] != 'B' || header[1] != 'M') {
            return nullptr;
        }
        const auto *info_header = reinterpret_cast<const BitmapInfoHeader *>(data + 14);
        if (info_header->bit_count != 24 && info_header->bit_count != 32) {
            return nullptr;
        }
        const auto bitmap = std::make_shared<Bitmap>(info_header->width, info_header->height);
        const uint8_t *ptr = data + 14 + sizeof(BitmapInfoHeader);
        // 每个像素的字节数 24位为3字节 32位为4字节
        uint32_t pixel_size = (info_header->bit_count + 7) / 8;
        // 每行字节数 4字节对齐
        uint32_t pitch = (pixel_size * info_header->width + 3) & (~3);
        for (int i = 0; i < info_header->height; i++) {
            auto *line = bitmap->line(info_header->height - i - 1);
            for (int j = 0; j < info_header->width; j += 4) {
                line[3] = 0xFF;
                std::memcpy(line, ptr, pixel_size);
                ptr += pixel_size;
            }
            const auto offset = (long) (pitch - pixel_size * info_header->width);
            ptr += offset;
        }
        return bitmap;
    }

    inline static std::shared_ptr<Bitmap> load_bmp(const char *filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            return nullptr;
        }
        file.seekg(0, std::ios::end);
        const auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        Buffer buffer(size);
        file.read(reinterpret_cast<char *>(buffer.data()), size);
        return load_from_buffer(buffer);
    }

    [[nodiscard]] inline std::shared_ptr<Buffer> save_to_buffer(bool with_alpha = false) const {
        Buffer buffer;
        uint32_t pixel_size = with_alpha ? 4 : 3;
        uint32_t pitch = (width_ * pixel_size + 3) & (~3);
        uint32_t file_size = 14 + sizeof(BitmapInfoHeader) + pitch * height_;
        uint32_t zero = 0;
        uint32_t offset = 14 + sizeof(BitmapInfoHeader);
        BitmapInfoHeader info_header{};
        info_header.size = sizeof(BitmapInfoHeader);
        info_header.width = width_;
        info_header.height = height_;
        info_header.planes = 1;
        info_header.bit_count = with_alpha ? 32 : 24;
        info_header.compression = 0;
        info_header.size_image = width_ * height_ * (with_alpha ? 4 : 3);
        info_header.x_pels_per_meter = 0x0B13;
        info_header.y_pels_per_meter = 0x0B13;
        info_header.clr_used = 0;
        info_header.clr_important = 0;
        buffer.resize(file_size);
        uint8_t *ptr = buffer.data();
        std::memcpy(ptr, "BM", 2);
        ptr += 2;
        std::memcpy(ptr, &file_size, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
        std::memcpy(ptr, &zero, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
        std::memcpy(ptr, &offset, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
        std::memcpy(ptr, &info_header, sizeof(BitmapInfoHeader));
        ptr += sizeof(BitmapInfoHeader);
        for (int i = 0; i < height_; i++) {
            auto *line = this->line(height_ - i - 1);
            uint32_t padding = pitch - width_ * pixel_size;
            for (int j = 0; j < width_; j++) {
                std::memcpy(ptr, line, pixel_size);
                ptr += pixel_size;
                line += 4;
            }
            for (int j = 0; j < padding; j++) {
                *ptr++ = 0;
            }
        }
        return std::make_shared<Buffer>(buffer);
    }

    inline bool save_bmp(const char *filename, bool with_alpha = false) const {
        auto buffer = save_to_buffer(with_alpha);
        if (!buffer) {
            return false;
        }
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        file.write(reinterpret_cast<const char *>(buffer->data()), static_cast<std::streamsize>(buffer->size()));
        return true;
    }


};

#endif //RENDERENGINE_BITMAP_HPP
