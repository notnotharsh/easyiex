#pragma once
#include <bits/stdc++.h>

template <typename T>
static constexpr T read_little_endian(const std::byte* p) {
    static_assert(std::is_integral_v<T>);
    T result = 0;
    for (size_t i = 0; i < sizeof(T); i++) result |= (static_cast<T>(std::to_integer<uint8_t>(p[i])) << (8 * i));
    return result;
}