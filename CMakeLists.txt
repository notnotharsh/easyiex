cmake_minimum_required(VERSION 4.1)
project(IEXBOOK VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(main
    src/main.cc
    src/processors/pcap_processor.cc
    src/processors/tops_processor.cc
    src/utils/endian_utils.cc
    src/utils/time_utils.cc
)

find_library(PCAP_LIB pcap REQUIRED)
target_link_libraries(main PRIVATE ${PCAP_LIB})