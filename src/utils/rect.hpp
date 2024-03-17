#pragma once

#include <type_traits>

namespace utils {

template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct Rect {
    T left = 0;
    T top = 0;
    T right = 0;
    T bottom = 0;
};

} // namespace utils
