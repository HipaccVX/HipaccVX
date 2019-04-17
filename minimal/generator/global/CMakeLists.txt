
add_executable (global_reduce global_reduce.cpp)
target_include_directories (global_reduce PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (global_reduce HipaVX_Cpp)


