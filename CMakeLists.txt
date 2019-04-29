cmake_minimum_required(VERSION 3.4.3)

project(HipaVX)
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")

set (CMAKE_CXX_STANDARD 11)

set(VX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/VX/)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(HIPAVX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX)
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/kernels/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})

file(GLOB_RECURSE HipaccGen_SRC ${HIPAVX_DIR}/hipacc_gen/*)
file(GLOB_RECURSE CppGen_SRC ${HIPAVX_DIR}/cpp_gen/*)

#add_library (HipaVX_Hipacc ${HipaVX_SRC} ${HipaccGen_SRC})
add_library (HipaVX_Cpp ${HipaVX_SRC} ${CppGen_SRC})

#add_executable (main_hipacc main.cpp)
#target_include_directories (main_hipacc PUBLIC ${VX_DIR} ${HIPAVX_DIR})
#target_link_libraries (main_hipacc HipaVX_Hipacc)

add_executable (main_cpp main.cpp)
target_include_directories (main_cpp PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (main_cpp HipaVX_Cpp)

add_subdirectory(minimal)
add_subdirectory(doc)
