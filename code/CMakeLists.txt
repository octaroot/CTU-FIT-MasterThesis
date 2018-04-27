cmake_minimum_required(VERSION 3.10)
project(code C)

set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)
find_package(OpenSSL REQUIRED)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -g -lssl -lcrypto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -lssl -lcrypto")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -lssl -lcrypto")

include_directories(${OPENSSL_INCLUDE_DIRs})

link_libraries(${OPENSSL_LIBRARIES})

file(GLOB_RECURSE plugin_files "plugins/*/*.c" "plugins/*/*.h")
file(GLOB_RECURSE src_files "src/*.c" "src/*.h")

add_executable(code
        ${src_files}
        ${plugin_files}
        )

add_custom_command(TARGET code
        POST_BUILD
        COMMAND /usr/bin/scp -i /home/martin/.ssh/id_rsa /home/martin/Desktop/CTU-FIT-MasterThesis/code/cmake-build-debug/code root@dip-test:/root/code
        )