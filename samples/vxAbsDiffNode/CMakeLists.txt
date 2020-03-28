add_executable (vxAbsDiffNode vxAbsDiffNode.cpp)
target_include_directories (vxAbsDiffNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxAbsDiffNode HipaVX_Hipacc)
create_hipacc_version(vxAbsDiffNode)
