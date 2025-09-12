#include "utils/endian_utils.h"

std::string Int64ToTicker(int64_t value) {
    std::string result(8, '\0');
    for (int i = 0; i < 8; i++) {
        uint8_t byte = static_cast<uint8_t>((value >> (8 * i) & 0xFF));
        result[i] = static_cast<char>(byte);
    }

    while (result.back() == ' ') result.pop_back();
    return result;
}