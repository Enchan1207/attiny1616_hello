cmake_minimum_required(VERSION 3.19)

add_executable(usart_recv_interrupt)

target_sources(usart_recv_interrupt PRIVATE
    main.c
)
