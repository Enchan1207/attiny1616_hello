cmake_minimum_required(VERSION 3.19)

add_executable(blink)

target_sources(blink PRIVATE
    main.c
)

target_compile_options(blink PRIVATE
    -Wall
)
