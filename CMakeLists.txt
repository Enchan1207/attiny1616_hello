cmake_minimum_required(VERSION 3.19)

project(cmake_avr_sdk
    VERSION 0.1.0
    DESCRIPTION "blink for ATtiny1616"
    LANGUAGES C
)

add_compile_options(
    -Wall
)

add_subdirectory(src)
