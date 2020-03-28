add_executable (vxDilate3x3Node vxDilate3x3Node.cpp)
target_include_directories (vxDilate3x3Node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxDilate3x3Node HipaVX_Hipacc)
create_hipacc_version(vxDilate3x3Node)
