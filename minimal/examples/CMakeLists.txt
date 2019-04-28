
add_executable (harris harris.cpp)
target_include_directories (harris PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (harris HipaVX_Cpp)


