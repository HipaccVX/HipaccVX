add_executable (vxChannelCombineNode vxChannelCombineNode.cpp)
target_include_directories (vxChannelCombineNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxChannelCombineNode HipaVX_Hipacc)
create_hipacc_version(vxChannelCombineNode)
