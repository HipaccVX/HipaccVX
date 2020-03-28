add_executable (vxOrNode vxOrNode.cpp)
target_include_directories (vxOrNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxOrNode HipaVX_Hipacc)
create_hipacc_version(vxOrNode)
