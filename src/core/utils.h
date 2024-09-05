//
// Created by qjming on 2024/9/5.
//

#ifndef RENDERENGINE_UTILS_H
#define RENDERENGINE_UTILS_H

// 绝对值
template<typename T>
inline T abs(T x) {
    return x < 0 ? -x : x;
}

// 求最大值
template<typename T>
inline T max(T a, T b) {
    return a > b ? a : b;
}

// 求最小值
template<typename T>
inline T min(T a, T b) {
    return a < b ? a : b;
}

// 判断两个浮点数是否相等
template<typename T>
inline bool near_equal(T a, T b, T epsilon) {
    return abs(a - b) < epsilon;
}

// 截取[min, max]之间的值
template<typename T>
inline T clamp(T x, T min, T max) {
    return x < min ? min : (x > max ? max : x);
}

// 截取[0, 1]之间的值
template<typename T>
inline T saturate(T x) {
    return clamp(x, T(0), T(1));
}

#endif //RENDERENGINE_UTILS_H
