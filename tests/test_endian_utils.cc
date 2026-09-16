#include <gtest/gtest.h>

#include <array>
#include <limits>

#include "utils/endian_utils.h"

TEST(ReadLittleEndian, Uint8SingleByte) {
    std::array<std::byte, 1> data{std::byte{0x42}};
    EXPECT_EQ(ReadLittleEndian<uint8_t>(data), 0x42u);
}

TEST(ReadLittleEndian, Uint16LittleEndian) {
    std::array<std::byte, 2> data{std::byte{0x01}, std::byte{0x02}};
    EXPECT_EQ(ReadLittleEndian<uint16_t>(data), 0x0201u);
}

TEST(ReadLittleEndian, Uint32LittleEndian) {
    std::array<std::byte, 4> data{std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}};
    EXPECT_EQ(ReadLittleEndian<uint32_t>(data), 0x04030201u);
}

TEST(ReadLittleEndian, Uint64LittleEndian) {
    std::array<std::byte, 8> data{
        std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
        std::byte{0x05}, std::byte{0x06}, std::byte{0x07}, std::byte{0x08}
    };
    EXPECT_EQ(ReadLittleEndian<uint64_t>(data), 0x0807060504030201ull);
}

TEST(ReadLittleEndian, AllZeros) {
    std::array<std::byte, 4> data{};
    EXPECT_EQ(ReadLittleEndian<uint32_t>(data), 0u);
}

TEST(ReadLittleEndian, MaxUint32) {
    std::array<std::byte, 4> data{std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}};
    EXPECT_EQ(ReadLittleEndian<uint32_t>(data), std::numeric_limits<uint32_t>::max());
}

TEST(ReadLittleEndian, WithOffset) {
    std::array<std::byte, 4> data{std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x02}};
    EXPECT_EQ(ReadLittleEndian<uint16_t>(data, 2), 0x0201u);
}

static int64_t TickerToInt64(const char* s) {
    int64_t v = 0;
    for (int i = 0; i < 8; i++)
        v |= (static_cast<int64_t>(static_cast<uint8_t>(s[i])) << (8 * i));
    return v;
}

TEST(Int64ToTicker, FourLetterTicker) {
    EXPECT_EQ(Int64ToTicker(TickerToInt64("AAPL    ")), "AAPL");
}

TEST(Int64ToTicker, OneLetterTicker) {
    EXPECT_EQ(Int64ToTicker(TickerToInt64("A       ")), "A");
}

TEST(Int64ToTicker, EightLetterTicker) {
    EXPECT_EQ(Int64ToTicker(TickerToInt64("ABCDEFGH")), "ABCDEFGH");
}

TEST(Int64ToTicker, StripsTrailingSpaces) {
    EXPECT_EQ(Int64ToTicker(TickerToInt64("SPY     ")), "SPY");
}

TEST(Int64ToTicker, HyphenatedTicker) {
    EXPECT_EQ(Int64ToTicker(TickerToInt64("BRK-B   ")), "BRK-B");
}
