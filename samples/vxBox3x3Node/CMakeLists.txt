add_executable (vxBox3x3Node vxBox3x3Node.cpp)
target_include_directories (vxBox3x3Node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxBox3x3Node HipaVX_Hipacc)
create_hipacc_version(vxBox3x3Node)
