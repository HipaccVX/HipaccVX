add_executable (vxSobel3x3Node vxSobel3x3Node.cpp)
target_include_directories (vxSobel3x3Node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxSobel3x3Node HipaVX_Hipacc)
create_hipacc_version(vxSobel3x3Node)
