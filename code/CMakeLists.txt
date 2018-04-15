cmake_minimum_required(VERSION 3.10)
project(code C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(code src/main.c src/common.h plugins/icmp/icmp.c plugins/icmp/icmp.h src/tun-device.c src/tun-device.h src/mux.c src/mux.h plugins/dns/dns.c plugins/dns/dns.h src/resolve.c src/resolve.h plugins/icmp/packet.c plugins/icmp/packet.h plugins/icmp/client-functions.c plugins/icmp/client-functions.h plugins/icmp/client-handlers.c plugins/icmp/client-handlers.h)