add_executable (vxSubtractNode vxSubtractNode.cpp)
target_include_directories (vxSubtractNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxSubtractNode HipaVX_Hipacc)
create_hipacc_version(vxSubtractNode)
