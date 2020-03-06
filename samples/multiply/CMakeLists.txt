add_executable (multiply multiply_node.cpp)
target_include_directories (multiply PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (multiply HipaVX_Hipacc)
create_hipacc_version(multiply)
