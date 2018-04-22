cmake_minimum_required(VERSION 3.10)
project(code C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)
find_package(OpenSSL REQUIRED)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -lssl -lcrypto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -lssl -lcrypto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lssl -lcrypto")

include_directories(${OPENSSL_INCLUDE_DIRs})

link_libraries(${OPENSSL_LIBRARIES})

add_executable(code src/main.c src/common.h plugins/icmp/icmp.c plugins/icmp/icmp.h src/tun-device.c src/tun-device.h src/mux.c src/mux.h plugins/dns/dns.c plugins/dns/dns.h src/resolve.c src/resolve.h plugins/icmp/packet.c plugins/icmp/packet.h plugins/icmp/client-functions.c plugins/icmp/client-functions.h plugins/icmp/client-handlers.c plugins/icmp/client-handlers.h plugins/icmp/server-handlers.c plugins/icmp/server-handlers.h plugins/icmp/server-functions.c plugins/icmp/server-functions.h src/keyfile.c src/keyfile.h src/auth.c src/auth.h)

add_custom_command(TARGET code
        POST_BUILD
        COMMAND /usr/bin/scp -i /home/martin/.ssh/id_rsa /home/martin/Desktop/CTU-FIT-MasterThesis/code/cmake-build-debug/code root@dip-test:/root/code
        )