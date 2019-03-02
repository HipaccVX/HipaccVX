cmake_minimum_required(VERSION 3.4.3)

set (CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE GLOB_RESULT ${CMAKE_CURRENT_SOURCE_DIR}/VX/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/kernels/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})

file(GLOB_RECURSE HipaccGen_SRC ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/hipacc_gen/*)
file(GLOB_RECURSE CppGen_SRC ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX/cpp_gen/*)

add_library (HipaVX_Hipacc ${HipaVX_SRC} ${HipaccGen_SRC})
add_library (HipaVX_Cpp ${HipaVX_SRC} ${CppGen_SRC})

add_executable (main_hipacc main.cpp)
target_include_directories (main_hipacc PUBLIC ${HipaVX_SRC})
target_link_libraries (main_hipacc HipaVX_Hipacc)

add_executable (main_cpp main.cpp)
target_include_directories (main_cpp PUBLIC ${HipaVX_SRC})
target_link_libraries (main_cpp HipaVX_Cpp)




add_subdirectory(minimal)
