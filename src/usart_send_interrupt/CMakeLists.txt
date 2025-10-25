cmake_minimum_required(VERSION 3.19)

add_executable(usart_send_interrupt)

target_sources(usart_send_interrupt PRIVATE
    main.c
)
