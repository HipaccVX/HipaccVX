add_executable (vxCopyNode vxCopyNode.cpp)
target_include_directories (vxCopyNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxCopyNode HipaVX_Hipacc)
create_hipacc_version(vxCopyNode)
