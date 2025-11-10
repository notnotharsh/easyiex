#pragma once
#include <bits/stdc++.h>

template <typename T>
static constexpr T ReadLittleEndian(std::span<const std::byte> p) {
    static_assert(std::is_integral_v<T>);
    T result = 0;
    for (size_t i = 0; i < sizeof(T); i++) result |= (static_cast<T>(std::to_integer<uint8_t>(p[i])) << (8 * i));
    return result;
}

template <typename T>
static constexpr T ReadLittleEndian(std::span<const std::byte> p, int offset) {
    return ReadLittleEndian<T>(p.subspan(offset, sizeof(T)));
}

std::string Int64ToTicker(int64_t value);