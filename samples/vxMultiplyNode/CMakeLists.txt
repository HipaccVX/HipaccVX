add_executable (vxMultiplyNode vxMultiplyNode.cpp)
target_include_directories (vxMultiplyNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxMultiplyNode HipaVX_Hipacc)
create_hipacc_version(vxMultiplyNode)
