#pragma once

#include "defines.hpp"

namespace utils {

template<typename T, usize N>
class Vector {
public:
    Vector() {
        for (usize i = 0; i < N; i++) {
            m_data[i] = T {};
        }
    }

    explicit Vector(T* data) {
        memcpy(m_data, data, N * sizeof(T));
    }

    template<typename... Args>
    explicit Vector(Args... args) {
        m_data = { static_cast<T>(args)... };
        static_assert(sizeof...(args) == N, "Incorrent  number of arguments provided.");
    }

    T* data() {
        return m_data;
    }

    T& operator[](usize index) {
        return m_data[index];
    }

    const T& operator[](usize index) const {
        return m_data[index];
    }

    Vector<T, N> operator*(T scalar) const {
        Vector<T, N> result;
        for (usize i = 0; i < N; i++) {
            result[i] = m_data[i] * scalar;
        }
        return result;
    }

private:
    T m_data[N];
};

} // namespace utils