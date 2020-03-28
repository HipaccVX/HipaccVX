add_executable (vxNotNode vxNotNode.cpp)
target_include_directories (vxNotNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxNotNode HipaVX_Hipacc)
create_hipacc_version(vxNotNode)
