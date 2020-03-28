add_executable (vxGaussian3x3Node vxGaussian3x3Node.cpp)
target_include_directories (vxGaussian3x3Node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxGaussian3x3Node HipaVX_Hipacc)
create_hipacc_version(vxGaussian3x3Node)
