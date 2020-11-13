cmake_minimum_required(VERSION 3.10)

project(HttpServer)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMKAE_CXX_COMPILER g++)

include_directories(./inc ./TinyLogger/inc ./TinyThreadPool/inc)

aux_source_directory(./src SRC)
aux_source_directory(./TinyLogger/src SRC1)
aux_source_directory(./TinyThreadPool/src SRC2)

add_executable(HttpServer.exe main.cc ${SRC} ${SRC1} ${SRC2})

target_link_libraries(HttpServer.exe pthread)

