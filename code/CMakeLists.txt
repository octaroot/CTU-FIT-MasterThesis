cmake_minimum_required(VERSION 3.9)
project(code C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)
find_package(OpenSSL REQUIRED)

FIND_PATH(
        SCTP_INCLUDE_DIR
        sctp.h
        /usr/include/ /usr/include/sctp/ /usr/include/netinet
        /usr/local/include/ /usr/local/include/sctp/ /usr/local/include/netinet/
)

FIND_LIBRARY(
        SCTP_LIBRARIES NAMES sctp
        PATHS /usr/lib/ /usr/local/lib/
)

link_libraries(${SCTP_LIBRARIES})

set(CUSTOM_FLAGS "-lssl -lcrypto -lsctp")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -g ${CUSTOM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ${CUSTOM_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} ${CUSTOM_FLAGS}")

include_directories(${OPENSSL_INCLUDE_DIRs})

link_libraries(${OPENSSL_LIBRARIES})

file(GLOB_RECURSE plugin_files "plugins/*/*.c" "plugins/*/*.h")
file(GLOB_RECURSE src_files "src/*.c" "src/*.h")

add_executable(code
        ${src_files}
        ${plugin_files}
        )

#add_custom_command(TARGET code
#        POST_BUILD
#        COMMAND /usr/bin/ssh -i /home/martin/.ssh/id_rsa root@dip-test 'killall -9 code 2>/dev/null || true'
#        COMMAND /usr/bin/scp -i /home/martin/.ssh/id_rsa /home/martin/Desktop/CTU-FIT-MasterThesis/code/cmake-build-debug/code root@dip-test:/root/code
#        COMMAND /usr/bin/ssh -i /home/martin/.ssh/id_rsa root@dip-test '/root/runcode.sh &'
#        )