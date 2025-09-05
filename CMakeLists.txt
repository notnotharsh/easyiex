cmake_minimum_required(VERSION 4.1)
project(IEXBOOK VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(pcap_processor src/pcap_processor.cpp)

find_library(PCAP_LIB pcap REQUIRED)
target_link_libraries(pcap_processor PRIVATE ${PCAP_LIB})