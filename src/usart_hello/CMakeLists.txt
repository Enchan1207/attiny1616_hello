cmake_minimum_required(VERSION 3.19)

add_executable(usart_hello)

target_sources(usart_hello PRIVATE
    main.c
)
