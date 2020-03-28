add_executable (vxErode3x3Node vxErode3x3Node.cpp)
target_include_directories (vxErode3x3Node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxErode3x3Node HipaVX_Hipacc)
create_hipacc_version(vxErode3x3Node)
