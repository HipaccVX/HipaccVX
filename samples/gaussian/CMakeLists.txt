add_executable (gaussian gaussian.cpp)
target_include_directories (gaussian PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (gaussian HipaVX_Hipacc)
create_hipacc_version(gaussian)
