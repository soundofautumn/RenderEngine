//
// Created by Autumn Sound on 2024/9/5.
//

//=====================================================================
// 数学库：矢量
// 说明：矢量是一个一维数组，用于表示一维空间的点和向量
// 来自：https://github.com/skywind3000/RenderHelp/blob/master/RenderHelp.h
//=====================================================================

#ifndef RENDERENGINE_VECTOR_HPP
#define RENDERENGINE_VECTOR_HPP

#include <cassert>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <sstream>

namespace RenderCore {

template <size_t N, typename T>
struct Vector {
    T m[N];

    constexpr inline Vector() {
        for (size_t i = 0; i < N; i++) {
            m[i] = T();
        }
    }

    constexpr inline Vector(const Vector<N, T> &v) {
        for (size_t i = 0; i < N; i++) {
            m[i] = v.m[i];
        }
    }

    constexpr inline Vector(Vector<N, T> &&v) noexcept {
        for (size_t i = 0; i < N; i++) {
            m[i] = std::move(v.m[i]);
        }
    }

    constexpr inline Vector(const std::initializer_list<T> &list) {
        assert(list.size() == N);
        size_t i = 0;
        for (const T &v : list) {
            m[i++] = v;
        }
    }

    constexpr inline Vector &operator=(const Vector<N, T> &v) {
        for (size_t i = 0; i < N; i++) {
            m[i] = v.m[i];
        }
        return *this;
    }

    constexpr inline Vector &operator=(Vector<N, T> &&v) noexcept {
        for (size_t i = 0; i < N; i++) {
            m[i] = std::move(v.m[i]);
        }
        return *this;
    }

    constexpr inline const T &operator[](size_t i) const {
        assert(i < N);
        return m[i];
    }

    constexpr inline T &operator[](size_t i) {
        assert(i < N);
        return m[i];
    }

    constexpr inline void load(const T *v) {
        for (size_t i = 0; i < N; i++) {
            m[i] = v[i];
        }
    }

    constexpr inline void store(T *v) {
        for (size_t i = 0; i < N; i++) {
            v[i] = m[i];
        }
    }
};

// 特化二维矢量
template <typename T>
struct Vector<2, T> {
    union {
        struct {
            T x, y;
        };
        struct {
            T u, v;
        };
        T m[2];
    };

    constexpr inline Vector() : x(T()), y(T()) {}

    constexpr inline Vector(T x, T y) : x(x), y(y) {}

    constexpr inline Vector(const Vector<2, T> &v) : x(v.x), y(v.y) {}

    constexpr inline Vector(Vector<2, T> &&v) noexcept : x(std::move(v.x)), y(std::move(v.y)) {}

    constexpr inline Vector(std::initializer_list<T> list) {
        assert(list.size() == 2);
        size_t i = 0;
        for (const T &v : list) {
            m[i++] = v;
        }
    }

    constexpr inline Vector &operator=(const Vector<2, T> &v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    constexpr inline Vector &operator=(Vector<2, T> &&v) noexcept {
        x = std::move(v.x);
        y = std::move(v.y);
        return *this;
    }

    constexpr inline const T &operator[](size_t i) const {
        assert(i < 2);
        return m[i];
    }

    constexpr inline T &operator[](size_t i) {
        assert(i < 2);
        return m[i];
    }

    constexpr inline void load(const T *v) {
        x = v[0];
        y = v[1];
    }

    constexpr inline void store(T *v) {
        v[0] = x;
        v[1] = y;
    }

    constexpr inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }

    constexpr inline Vector<3, T> xy1() const { return Vector<3, T>(x, y, 1); }

    constexpr inline Vector<4, T> xy11() const { return Vector<4, T>(x, y, 1, 1); }
};

// 特化三维矢量
template <typename T>
struct Vector<3, T> {
    union {
        struct {
            T x, y, z;
        };
        struct {
            T r, g, b;
        };
        T m[3];
    };

    constexpr inline Vector() : x(T()), y(T()), z(T()) {}

    constexpr inline Vector(T x, T y, T z) : x(x), y(y), z(z) {}

    constexpr inline Vector(const Vector<3, T> &v) : x(v.x), y(v.y), z(v.z) {}

    constexpr inline Vector(Vector<3, T> &&v) noexcept
        : x(std::move(v.x)), y(std::move(v.y)), z(std::move(v.z)) {}

    constexpr inline Vector(std::initializer_list<T> list) {
        assert(list.size() == 3);
        size_t i = 0;
        for (const T &v : list) {
            m[i++] = v;
        }
    }

    constexpr inline Vector &operator=(const Vector<3, T> &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    constexpr inline Vector &operator=(Vector<3, T> &&v) noexcept {
        x = std::move(v.x);
        y = std::move(v.y);
        z = std::move(v.z);
        return *this;
    }

    constexpr inline const T &operator[](size_t i) const {
        assert(i < 3);
        return m[i];
    }

    constexpr inline T &operator[](size_t i) {
        assert(i < 3);
        return m[i];
    }

    constexpr inline void load(const T *v) {
        x = v[0];
        y = v[1];
        z = v[2];
    }

    constexpr inline void store(T *v) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }

    constexpr inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }

    constexpr inline Vector<3, T> xyz() const { return Vector<3, T>(x, y, z); }

    constexpr inline Vector<4, T> xyz1() const { return Vector<4, T>(x, y, z, 1); }
};

// 特化四维矢量
template <typename T>
struct Vector<4, T> {
    union {
        struct {
            T x, y, z, w;
        };
        struct {
            T r, g, b, a;
        };
        T m[4];
    };

    constexpr inline Vector() : x(T()), y(T()), z(T()), w(T()) {}

    constexpr inline Vector(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    constexpr inline Vector(const Vector<4, T> &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    constexpr inline Vector(Vector<4, T> &&v) noexcept
        : x(std::move(v.x)), y(std::move(v.y)), z(std::move(v.z)), w(std::move(v.w)) {}

    constexpr inline Vector(std::initializer_list<T> list) {
        assert(list.size() == 4);
        size_t i = 0;
        for (const T &v : list) {
            m[i++] = v;
        }
    }

    constexpr inline Vector &operator=(const Vector<4, T> &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    constexpr inline Vector &operator=(Vector<4, T> &&v) noexcept {
        x = std::move(v.x);
        y = std::move(v.y);
        z = std::move(v.z);
        w = std::move(v.w);
        return *this;
    }

    constexpr inline const T &operator[](size_t i) const {
        assert(i < 4);
        return m[i];
    }

    constexpr inline T &operator[](size_t i) {
        assert(i < 4);
        return m[i];
    }

    constexpr inline void load(const T *v) {
        x = v[0];
        y = v[1];
        z = v[2];
        w = v[3];
    }

    constexpr inline void store(T *v) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
        v[3] = w;
    }

    constexpr inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }

    constexpr inline Vector<3, T> xyz() const { return Vector<3, T>(x, y, z); }

    constexpr inline Vector<4, T> xyzw() const { return Vector<4, T>(x, y, z, w); }
};

//---------------------------------------------------------------------
// 数学库：矢量运算
//---------------------------------------------------------------------

// = (+a)
template <size_t N, typename T>
constexpr inline Vector<N, T> operator+(const Vector<N, T> &a) {
    return a;
}

// = (-a)
template <size_t N, typename T>
constexpr inline Vector<N, T> operator-(const Vector<N, T> &a) {
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = -a[i];
    return b;
}

// = (a == b) ? true : false
template <size_t N, typename T>
constexpr inline bool operator==(const Vector<N, T> &a, const Vector<N, T> &b) {
    for (size_t i = 0; i < N; i++)
        if (a[i] != b[i]) return false;
    return true;
}

// = (a != b)? true : false
template <size_t N, typename T>
constexpr inline bool operator!=(const Vector<N, T> &a, const Vector<N, T> &b) {
    return !(a == b);
}

// = a + b
template <size_t N, typename T>
constexpr inline Vector<N, T> operator+(const Vector<N, T> &a, const Vector<N, T> &b) {
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] + b[i];
    return c;
}

// = a - b
template <size_t N, typename T>
constexpr inline Vector<N, T> operator-(const Vector<N, T> &a, const Vector<N, T> &b) {
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] - b[i];
    return c;
}

// = a * b，不是点乘也不是叉乘，而是各个元素分别相乘，色彩计算时有用
template <size_t N, typename T>
constexpr inline Vector<N, T> operator*(const Vector<N, T> &a, const Vector<N, T> &b) {
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] * b[i];
    return c;
}

// = a / b，各个元素相除
template <size_t N, typename T>
constexpr inline Vector<N, T> operator/(const Vector<N, T> &a, const Vector<N, T> &b) {
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = a[i] / b[i];
    return c;
}

// = a * x
template <size_t N, typename T>
constexpr inline Vector<N, T> operator*(const Vector<N, T> &a, T x) {
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
    return b;
}

// = x * a
template <size_t N, typename T>
constexpr inline Vector<N, T> operator*(T x, const Vector<N, T> &a) {
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = a[i] * x;
    return b;
}

// = a / x
template <size_t N, typename T>
constexpr inline Vector<N, T> operator/(const Vector<N, T> &a, T x) {
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = a[i] / x;
    return b;
}

// = x / a
template <size_t N, typename T>
constexpr inline Vector<N, T> operator/(T x, const Vector<N, T> &a) {
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) b[i] = x / a[i];
    return b;
}

// a += b
template <size_t N, typename T>
constexpr inline Vector<N, T> &operator+=(Vector<N, T> &a, const Vector<N, T> &b) {
    for (size_t i = 0; i < N; i++) a[i] += b[i];
    return a;
}

// a -= b
template <size_t N, typename T>
constexpr inline Vector<N, T> &operator-=(Vector<N, T> &a, const Vector<N, T> &b) {
    for (size_t i = 0; i < N; i++) a[i] -= b[i];
    return a;
}

// a *= b
template <size_t N, typename T>
constexpr inline Vector<N, T> &operator*=(Vector<N, T> &a, const Vector<N, T> &b) {
    for (size_t i = 0; i < N; i++) a[i] *= b[i];
    return a;
}

// a /= b
template <size_t N, typename T>
constexpr inline Vector<N, T> &operator/=(Vector<N, T> &a, const Vector<N, T> &b) {
    for (size_t i = 0; i < N; i++) a[i] /= b[i];
    return a;
}

// a *= x
template <size_t N, typename T>
constexpr inline Vector<N, T> &operator*=(Vector<N, T> &a, T x) {
    for (size_t i = 0; i < N; i++) a[i] *= x;
    return a;
}

// a /= x
template <size_t N, typename T>
constexpr inline Vector<N, T> &operator/=(Vector<N, T> &a, T x) {
    for (size_t i = 0; i < N; i++) a[i] /= x;
    return a;
}

//---------------------------------------------------------------------
// 数学库：矢量函数
//---------------------------------------------------------------------

// 不同维度的矢量转换
template <size_t N1, size_t N2, typename T>
constexpr inline Vector<N1, T> vector_convert(const Vector<N2, T> &a, T fill = 1) {
    Vector<N1, T> b;
    for (size_t i = 0; i < N1; i++) b[i] = (i < N2) ? a[i] : fill;
    return b;
}

// = |a| ^ 2
template <size_t N, typename T>
constexpr inline T vector_length_square(const Vector<N, T> &a) {
    T sum = 0;
    for (size_t i = 0; i < N; i++) sum += a[i] * a[i];
    return sum;
}

// = |a|
template <size_t N, typename T>
constexpr inline T vector_length(const Vector<N, T> &a) {
    return sqrt(vector_length_square(a));
}

// = |a| , 特化 float 类型，使用 sqrtf
template <size_t N>
constexpr inline float vector_length(const Vector<N, float> &a) {
    return sqrtf(vector_length_square(a));
}

// = a / |a|
template <size_t N, typename T>
constexpr inline Vector<N, T> vector_normalize(const Vector<N, T> &a) {
    return a / vector_length(a);
}

// 矢量点乘
template <size_t N, typename T>
constexpr inline T vector_dot(const Vector<N, T> &a, const Vector<N, T> &b) {
    T sum = 0;
    for (size_t i = 0; i < N; i++) sum += a[i] * b[i];
    return sum;
}

// 二维矢量叉乘，得到标量
template <typename T>
constexpr inline T vector_cross(const Vector<2, T> &a, const Vector<2, T> &b) {
    return a.x * b.y - a.y * b.x;
}

// 三维矢量叉乘，得到新矢量
template <typename T>
constexpr inline Vector<3, T> vector_cross(const Vector<3, T> &a, const Vector<3, T> &b) {
    return Vector<3, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// 四维矢量叉乘：前三维叉乘，后一位保留
template <typename T>
constexpr inline Vector<4, T> vector_cross(const Vector<4, T> &a, const Vector<4, T> &b) {
    return Vector<4, T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w);
}

// = a + (b - a) * t
template <size_t N, typename T>
constexpr inline Vector<N, T> vector_lerp(const Vector<N, T> &a, const Vector<N, T> &b, float t) {
    return a + (b - a) * t;
}

// 各个元素取最大值
template <size_t N, typename T>
constexpr inline Vector<N, T> vector_max(const Vector<N, T> &a, const Vector<N, T> &b) {
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = (a[i] > b[i]) ? a[i] : b[i];
    return c;
}

// 各个元素取最小值
template <size_t N, typename T>
constexpr inline Vector<N, T> vector_min(const Vector<N, T> &a, const Vector<N, T> &b) {
    Vector<N, T> c;
    for (size_t i = 0; i < N; i++) c[i] = (a[i] < b[i]) ? a[i] : b[i];
    return c;
}

// 将矢量的值控制在 minx/maxx 范围内
template <size_t N, typename T>
constexpr inline Vector<N, T> vector_between(
    const Vector<N, T> &minx, const Vector<N, T> &maxx, const Vector<N, T> &x) {
    return vector_min(vector_max(minx, x), maxx);
}

// 判断矢量是否接近
template <size_t N, typename T>
constexpr inline bool vector_near(const Vector<N, T> &a, const Vector<N, T> &b, T dist) {
    return (vector_length_square(a - b) <= dist);
}

// 判断两个单精度矢量是否近似
template <size_t N>
constexpr inline bool vector_near_equal(
    const Vector<N, float> &a, const Vector<N, float> &b, float e = 0.0001) {
    return vector_near(a, b, e);
}

// 判断两个双精度矢量是否近似
template <size_t N>
constexpr inline bool vector_near_equal(
    const Vector<N, double> &a, const Vector<N, double> &b, double e = 0.0000001) {
    return vector_near(a, b, e);
}

// 矢量值元素范围裁剪
template <size_t N, typename T>
constexpr inline Vector<N, T> vector_clamp(const Vector<N, T> &a, T minx = 0, T maxx = 1) {
    Vector<N, T> b;
    for (size_t i = 0; i < N; i++) {
        T x = (a[i] < minx) ? minx : a[i];
        b[i] = (x > maxx) ? maxx : x;
    }
    return b;
}

// 输出到文本流
template <size_t N, typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector<N, T> &a) {
    os << "[";
    for (size_t i = 0; i < N; i++) {
        os << a[i];
        if (i < N - 1) os << ", ";
    }
    os << "]";
    return os;
}

// 输出成字符串
template <size_t N, typename T>
inline std::string vector_repr(const Vector<N, T> &a) {
    std::stringstream ss;
    ss << a;
    return ss.str();
}

// 类型别名
using Vector2f = Vector<2, float>;
using Vector3f = Vector<3, float>;
using Vector4f = Vector<4, float>;
using Vector2d = Vector<2, double>;
using Vector3d = Vector<3, double>;
using Vector4d = Vector<4, double>;
using Vector2i = Vector<2, int>;
using Vector3i = Vector<3, int>;
using Vector4i = Vector<4, int>;

}  // namespace RenderCore

#endif  //RENDERENGINE_VECTOR_HPP
