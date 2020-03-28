add_executable (vxThresholdNode vxThresholdNode.cpp)
target_include_directories (vxThresholdNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxThresholdNode HipaVX_Hipacc)
create_hipacc_version(vxThresholdNode)
