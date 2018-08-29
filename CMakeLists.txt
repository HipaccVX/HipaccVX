cmake_minimum_required(VERSION 3.11)


set (CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE HipaVX_SRC ${CMAKE_CURRENT_SOURCE_DIR}/VX/hipaVX/*)
add_library (HipaVX ${HipaVX_SRC})



add_executable (main main.cpp)
target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VX)
target_link_libraries (main HipaVX)
