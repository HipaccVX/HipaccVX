cmake_minimum_required(VERSION 3.4.3)

project(HipaVX)
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")

set (CMAKE_CXX_STANDARD 11)

set(VX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/VX/)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(HIPAVX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX)
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/kernels/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/dsl/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/graph/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/gen/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})


set(COMMON_INC_DIRS ${VX_DIR} ${HIPAVX_DIR} ${ROOT_DIR})

file(GLOB_RECURSE HipaccGen_SRC ${HIPAVX_DIR}/gen/hipacc_*)

add_library (HipaVX_Hipacc ${HipaVX_SRC} ${HipaccGen_SRC})

add_executable (main_hipacc main.cpp)
target_include_directories (main_hipacc PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (main_hipacc HipaVX_Hipacc)

add_subdirectory(tests)
