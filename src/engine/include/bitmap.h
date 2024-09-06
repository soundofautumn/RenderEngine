//
// Created by Autumn Sound on 2024/9/5.
//

#ifndef RENDERENGINE_BITMAP_H
#define RENDERENGINE_BITMAP_H

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>

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
    inline static std::shared_ptr<Bitmap> load_bmp(const char *filename) {
        FILE * file = fopen(filename, "rb");
        if (!file) {
            return nullptr;
        }

        uint8_t header[14];
        size_t hr = fread(header, sizeof(uint8_t), 14, file);
        if (hr != 14 || header[0] != 'B' || header[1] != 'M') {
            fclose(file);
            return nullptr;
        }

        BitmapInfoHeader info_header{};
        hr = fread(&info_header, sizeof(BitmapInfoHeader), 1, file);
        if (hr != sizeof(BitmapInfoHeader)) {
            fclose(file);
            return nullptr;
        }

        if (info_header.bit_count != 24 && info_header.bit_count != 32) {
            fclose(file);
            return nullptr;
        }

        const auto bitmap = std::make_shared<Bitmap>(info_header.width, info_header.height);
        fseek(file, 14 + sizeof(BitmapInfoHeader), SEEK_SET);

        // 每个像素的字节数 24位为3字节 32位为4字节
        uint32_t pixel_size = (info_header.bit_count + 7) / 8;
        // 每行字节数 4字节对齐
        uint32_t pitch = (pixel_size * info_header.width + 3) & (~3);
        for (int i = 0; i < info_header.height; i++) {
            auto *line = bitmap->line(info_header.height - i - 1);
            for (int j = 0; j < info_header.width; j += 4) {
                line[3] = 0xFF;
                fread(line, sizeof(uint8_t), pixel_size, file);
            }
            const auto offset = (long) (pitch - pixel_size * info_header.width);
            fseek(file, offset, SEEK_CUR);
        }
        return bitmap;
    }

    inline bool save_bmp(const char *filename, bool with_alpha = false) {
        FILE * file = fopen(filename, "wb");
        if (!file) {
            return false;
        }
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
        fwrite("BM", sizeof(uint8_t), 2, file);
        fwrite(&file_size, sizeof(uint32_t), 1, file);
        fwrite(&zero, sizeof(uint32_t), 1, file);
        fwrite(&offset, sizeof(uint32_t), 1, file);
        fwrite(&info_header, sizeof(BitmapInfoHeader), 1, file);
        for (int i = 0; i < height_; i++) {
            auto *line = this->line(height_ - i - 1);
            uint32_t padding = pitch - width_ * pixel_size;
            for (int j = 0; j < width_; j++) {
                fwrite(line, sizeof(uint8_t), pixel_size, file);
                line += 4;
            }
            for (int j = 0; j < padding; j++) {
                fputc(0, file);
            }
        }
        fclose(file);
        return true;
    }


};

#endif //RENDERENGINE_BITMAP_H
