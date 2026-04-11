cmake_minimum_required(VERSION 3.16)
project(easyiex VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(main
    include/utils/structures.h
    src/main.cc
    src/processors/tops_processor.cc
    src/utils/endian_utils.cc
    src/utils/time_utils.cc
    src/persistence/quote_update_table_builder.cc
)

find_library(PCAP_LIB pcap REQUIRED)
target_link_libraries(main PRIVATE ${PCAP_LIB})

find_package(Arrow REQUIRED)
target_link_libraries(main PRIVATE Arrow::arrow_shared)

find_package(Parquet REQUIRED)
target_link_libraries(main PRIVATE Parquet::parquet_shared)