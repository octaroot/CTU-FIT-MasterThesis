cmake_minimum_required(VERSION 3.10)
project(code C)

set(CMAKE_C_STANDARD 11)

add_executable(code src/main.c src/common.h src/tun-device.c src/tun-device.h)