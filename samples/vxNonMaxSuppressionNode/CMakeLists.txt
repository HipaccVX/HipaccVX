add_executable (vxNonMaxSuppressionNode vxNonMaxSuppressionNode.cpp)
target_include_directories (vxNonMaxSuppressionNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxNonMaxSuppressionNode HipaVX_Hipacc)
create_hipacc_version(vxNonMaxSuppressionNode)
