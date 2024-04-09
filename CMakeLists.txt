cmake_minimum_required(VERSION 3.20)
project(ImHexPlugin)

if ($ENV{IMHEX_SDK_PATH} STREQUAL "")
    message(FATAL_ERROR "The IMHEX_SDK_PATH environment variable is not set")
elseif (NOT EXISTS $ENV{IMHEX_SDK_PATH})
    message(FATAL_ERROR "The IMHEX_SDK_PATH environment variable doesn't contain a valid path")
endif()
add_subdirectory($ENV{IMHEX_SDK_PATH} ImHexSDK)

find_package(ZLIB REQUIRED)

add_imhex_plugin(
    NAME
        imhex_data_processor_zlib
    SOURCES
        source/main.cpp
        source/content/nodes/deflate_node.cpp
    INCLUDES
        include
    LIBRARIES
        ZLIB::ZLIB
)
