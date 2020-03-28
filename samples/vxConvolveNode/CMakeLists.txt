add_executable (vxConvolveNode vxConvolveNode.cpp)
target_include_directories (vxConvolveNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxConvolveNode HipaVX_Hipacc)
create_hipacc_version(vxConvolveNode)
