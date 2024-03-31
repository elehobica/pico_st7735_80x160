cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PROJECT_NAME "pico_st7735_80x160_test" C CXX)
project(${PROJECT_NAME})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(.. pico_st7735_80x160)

add_executable(${PROJECT_NAME}
    main.c
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME} PRIVATE
    hardware_uart
    pico_stdlib
    pico_st7735_80x160
)
pico_add_extra_outputs(${PROJECT_NAME})
