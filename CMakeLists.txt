cmake_minimum_required(VERSION 3.4.3)

set (CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE HipaVX_SRC ${CMAKE_CURRENT_SOURCE_DIR}/VX/*)
file(GLOB_RECURSE HipaVX_SRC ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/hipacc_gen/*)
file(GLOB_RECURSE HipaVX_SRC ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/*)
add_library (HipaVX ${HipaVX_SRC})

add_executable (main main.cpp)
target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VX ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/hipacc_gen)
target_link_libraries (main HipaVX)
