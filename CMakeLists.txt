# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED False)

idf_component_register(
    SRCS "heatshrink_encoder.c" "heatshrink_encoder_32bit.cpp"
         "heatshrink_decoder.c" "heatshrink_decoder_32bit.cpp"
    INCLUDE_DIRS "."
    PRIV_INCLUDE_DIRS "private"
)

# Note: Configuration is primarily handled through sdkconfig.h (generated from Kconfig)
# which is automatically included in ESP-IDF builds. The heatshrink_config.h header
# reads CONFIG_* defines from sdkconfig.h.
# 
# The explicit definitions below are kept for backwards compatibility and to ensure
# the preprocessor can use these values before including heatshrink_config.h, but
# they are redundant with sdkconfig.h in most cases.

if(CONFIG_HEATSHRINK_32BIT)
    add_definitions(-DHEATSHRINK_32BIT=1)
else()
    add_definitions(-DHEATSHRINK_32BIT=0)
endif()

if(CONFIG_HEATSHRINK_USE_INDEX)
    add_definitions(-DHEATSHRINK_USE_INDEX=1)
else()
    add_definitions(-DHEATSHRINK_USE_INDEX=0)    
endif()