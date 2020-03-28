add_executable (vxMedian3x3Node vxMedian3x3Node.cpp)
target_include_directories (vxMedian3x3Node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxMedian3x3Node HipaVX_Hipacc)
create_hipacc_version(vxMedian3x3Node)
