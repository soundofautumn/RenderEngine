//
// Created by 邱姜铭 on 2024/9/5.
//
#include "bitmap.h"

int main() {
    Bitmap bitmap(800, 600);
    bitmap.fill(0x00FF0000);
    if (!bitmap.save_bmp("test.bmp")){
        return 1;
    };

}