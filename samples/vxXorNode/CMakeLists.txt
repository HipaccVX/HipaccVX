add_executable (vxXorNode vxXorNode.cpp)
target_include_directories (vxXorNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxXorNode HipaVX_Hipacc)
create_hipacc_version(vxXorNode)
