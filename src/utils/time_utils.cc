#include "utils/time_utils.h"

std::string ns_to_timestamp(int64_t ns) {
    std::chrono::system_clock::time_point tp{std::chrono::nanoseconds{ns}};
    const std::chrono::time_zone* ny_tz = std::chrono::locate_zone("America/New_York");
    std::chrono::zoned_time ny_time{ny_tz, tp};

    return std::format("{:%Y-%m-%d %H:%M:%S}", ny_time);
}