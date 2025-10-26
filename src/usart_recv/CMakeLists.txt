cmake_minimum_required(VERSION 3.19)

add_executable(usart_recv)

target_sources(usart_recv PRIVATE
    main.c
)
