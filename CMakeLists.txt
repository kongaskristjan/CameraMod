cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)
project(camera C CXX ASM)
pico_sdk_init()

add_executable(camera src/main.cpp)
pico_enable_stdio_usb(camera 1)
pico_enable_stdio_uart(camera 1)
pico_add_extra_outputs(camera)
target_link_libraries(camera pico_stdlib hardware_adc)
