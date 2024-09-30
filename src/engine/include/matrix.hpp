//
// Created by Autumn Sound on 2024/9/5.
//

//=====================================================================
// 数学库：矩阵
// 说明：矩阵是一个二维数组，用于表示二维空间的变换和计算
// 来自：https://github.com/skywind3000/RenderHelp/blob/master/RenderHelp.h
//=====================================================================

#ifndef RENDERENGINE_MATRIX_H
#define RENDERENGINE_MATRIX_H

#include <cassert>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <memory>

#include "vector.hpp"

namespace RenderCore {

//---------------------------------------------------------------------
// 数学库：矩阵
//---------------------------------------------------------------------
template <size_t ROW, size_t COL, typename T>
struct Matrix {
    T m[ROW][COL];

    constexpr inline Matrix() = default;

    constexpr inline Matrix(const Matrix &other) { memcpy(m, other.m, sizeof(m)); }

    constexpr inline Matrix(Matrix &&other) noexcept {
        for (size_t i = 0; i < ROW; i++) {
            for (size_t j = 0; j < COL; j++) {
                m[i][j] = std::move(other.m[i][j]);
            }
        }
    }

    constexpr inline Matrix(std::initializer_list<std::initializer_list<T>> list) {
        size_t i = 0;
        for (auto &row : list) {
            size_t j = 0;
            for (auto &col : row) {
                m[i][j] = col;
                j++;
            }
            i++;
        }
    }

    constexpr inline Matrix &operator=(const Matrix &other) {
        if (this != &other) memcpy(m, other.m, sizeof(m));
        return *this;
    }

    constexpr inline Matrix &operator=(Matrix &&other) noexcept {
        if (this != &other) {
            for (size_t i = 0; i < ROW; i++) {
                for (size_t j = 0; j < COL; j++) {
                    m[i][j] = std::move(other.m[i][j]);
                }
            }
        }
        return *this;
    }

    constexpr inline const T *operator[](size_t row) const {
        assert(row < ROW);
        return m[row];
    }

    constexpr inline T *operator[](size_t row) {
        assert(row < ROW);
        return m[row];
    }

    constexpr inline Vector<COL, T> row(size_t r) const {
        assert(r < ROW);
        Vector<COL, T> v;
        for (size_t i = 0; i < COL; i++) {
            v[i] = m[r][i];
        }
        return v;
    }

    constexpr inline Vector<ROW, T> col(size_t c) const {
        assert(c < COL);
        Vector<ROW, T> v;
        for (size_t i = 0; i < ROW; i++) {
            v[i] = m[i][c];
        }
        return v;
    }

    constexpr inline void set_row(size_t r, const Vector<COL, T> &v) {
        assert(r < ROW);
        for (size_t i = 0; i < COL; i++) {
            m[r][i] = v[i];
        }
    }

    constexpr inline void set_col(size_t c, const Vector<ROW, T> &v) {
        assert(c < COL);
        for (size_t i = 0; i < ROW; i++) {
            m[i][c] = v[i];
        }
    }

    // 转置矩阵
    constexpr inline Matrix<ROW, COL, T> transpose() const {
        Matrix<COL, ROW, T> res;
        for (size_t i = 0; i < ROW; i++) {
            for (size_t j = 0; j < COL; j++) {
                res.m[j][i] = m[i][j];
            }
        }
        return res;
    };

    // 单位矩阵
    inline static Matrix<ROW, COL, T> identity() {
        Matrix<ROW, COL, T> res;
        for (size_t i = 0; i < ROW; i++) {
            for (size_t j = 0; j < COL; j++) {
                res.m[i][j] = i == j ? 1 : 0;
            }
        }
        return res;
    }

    // 零矩阵
    inline static Matrix<ROW, COL, T> zero() {
        Matrix<ROW, COL, T> res;
        for (size_t i = 0; i < ROW; i++) {
            for (size_t j = 0; j < COL; j++) {
                res.m[i][j] = 0;
            }
        }
        return res;
    }
};

//---------------------------------------------------------------------
// 数学库：矩阵运算
//---------------------------------------------------------------------
template <size_t ROW, size_t COL, typename T>
constexpr inline bool operator==(const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b) {
    for (size_t r = 0; r < ROW; r++) {
        for (size_t c = 0; c < COL; c++) {
            if (a.m[r][c] != b.m[r][c]) return false;
        }
    }
    return true;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline bool operator!=(const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b) {
    return !(a == b);
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator+(const Matrix<ROW, COL, T> &src) {
    return src;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator-(const Matrix<ROW, COL, T> &src) {
    Matrix<ROW, COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < COL; i++) out.m[j][i] = -src.m[j][i];
    }
    return out;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator+(
    const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b) {
    Matrix<ROW, COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < COL; i++) out.m[j][i] = a.m[j][i] + b.m[j][i];
    }
    return out;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator-(
    const Matrix<ROW, COL, T> &a, const Matrix<ROW, COL, T> &b) {
    Matrix<ROW, COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < COL; i++) out.m[j][i] = a.m[j][i] - b.m[j][i];
    }
    return out;
}

template <size_t ROW, size_t COL, size_t NEW_COL, typename T>
constexpr inline Matrix<ROW, NEW_COL, T> operator*(
    const Matrix<ROW, COL, T> &a, const Matrix<COL, NEW_COL, T> &b) {
    Matrix<ROW, NEW_COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < NEW_COL; i++) {
            out.m[j][i] = vector_dot(a.row(j), b.col(i));
        }
    }
    return out;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator*(const Matrix<ROW, COL, T> &a, T x) {
    Matrix<ROW, COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < COL; i++) {
            out.m[j][i] = a.m[j][i] * x;
        }
    }
    return out;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator/(const Matrix<ROW, COL, T> &a, T x) {
    Matrix<ROW, COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < COL; i++) {
            out.m[j][i] = a.m[j][i] / x;
        }
    }
    return out;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator*(T x, const Matrix<ROW, COL, T> &a) {
    return (a * x);
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Matrix<ROW, COL, T> operator/(T x, const Matrix<ROW, COL, T> &a) {
    Matrix<ROW, COL, T> out;
    for (size_t j = 0; j < ROW; j++) {
        for (size_t i = 0; i < COL; i++) {
            out.m[j][i] = x / a.m[j][i];
        }
    }
    return out;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Vector<COL, T> operator*(const Vector<ROW, T> &a, const Matrix<ROW, COL, T> &m) {
    Vector<COL, T> b;
    for (size_t i = 0; i < COL; i++) b[i] = vector_dot(a, m.col(i));
    return b;
}

template <size_t ROW, size_t COL, typename T>
constexpr inline Vector<ROW, T> operator*(const Matrix<ROW, COL, T> &m, const Vector<COL, T> &a) {
    Vector<ROW, T> b;
    for (size_t i = 0; i < ROW; i++) b[i] = vector_dot(a, m.row(i));
    return b;
}

template <size_t ROW, size_t COL, typename T, typename Other>
constexpr inline Matrix<ROW, COL, T> operator+=(Matrix<ROW, COL, T> &a, const Other &b) {
    a = a + b;
    return a;
}

template <size_t ROW, size_t COL, typename T, typename Other>
constexpr inline Matrix<ROW, COL, T> operator-=(Matrix<ROW, COL, T> &a, const Other &b) {
    a = a - b;
    return a;
}

template <size_t ROW, size_t COL, typename T, typename Other>
constexpr inline Matrix<ROW, COL, T> operator*=(Matrix<ROW, COL, T> &a, const Other &b) {
    a = a * b;
    return a;
}

template <size_t ROW, size_t COL, typename T, typename Other>
constexpr inline Matrix<ROW, COL, T> operator/=(Matrix<ROW, COL, T> &a, const Other &b) {
    a = a / b;
    return a;
}

//---------------------------------------------------------------------
// 数学库：行列式和逆矩阵等，光照计算有用
//---------------------------------------------------------------------

// 行列式求值：一阶
template <typename T>
constexpr inline T matrix_det(const Matrix<1, 1, T> &m) {
    return m[0][0];
}

// 行列式求值：二阶
template <typename T>
constexpr inline T matrix_det(const Matrix<2, 2, T> &m) {
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

// 行列式求值：多阶行列式，即第一行同他们的余子式相乘求和
template <size_t N, typename T>
constexpr inline T matrix_det(const Matrix<N, N, T> &m) {
    T sum = 0;
    for (size_t i = 0; i < N; i++) sum += m[0][i] * matrix_cofactor(m, 0, i);
    return sum;
}

// 余子式：一阶
template <typename T>
constexpr inline T matrix_cofactor(const Matrix<1, 1, T> &, size_t, size_t) {
    return 0;
}

// 多阶余子式：即删除特定行列的子式的行列式值
template <size_t N, typename T>
constexpr inline T matrix_cofactor(const Matrix<N, N, T> &m, size_t row, size_t col) {
    return matrix_det(m.GetMinor(row, col)) * (((row + col) % 2) ? -1 : 1);
}

// 伴随矩阵：即余子式矩阵的转置
template <size_t N, typename T>
constexpr inline Matrix<N, N, T> matrix_adjoint(const Matrix<N, N, T> &m) {
    Matrix<N, N, T> ret;
    for (size_t j = 0; j < N; j++) {
        for (size_t i = 0; i < N; i++) ret[j][i] = matrix_cofactor(m, i, j);
    }
    return ret;
}

// 求逆矩阵：使用伴随矩阵除以行列式的值得到
template <size_t N, typename T>
constexpr inline Matrix<N, N, T> matrix_invert(const Matrix<N, N, T> &m) {
    Matrix<N, N, T> ret = matrix_adjoint(m);
    T det = vector_dot(m.Row(0), ret.Col(0));
    return ret / det;
}

// 输出到文本流
template <size_t ROW, size_t COL, typename T>
inline std::ostream &operator<<(std::ostream &os, const Matrix<ROW, COL, T> &m) {
    for (size_t r = 0; r < ROW; r++) {
        Vector<COL, T> row = m.Row(r);
        os << row << std::endl;
    }
    return os;
}

using Matrix2f = Matrix<2, 2, float>;
using Matrix3f = Matrix<3, 3, float>;
using Matrix4f = Matrix<4, 4, float>;

}  // namespace RenderCore

#endif  //RENDERENGINE_MATRIX_H
