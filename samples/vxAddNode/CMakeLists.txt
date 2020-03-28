add_executable (vxAddNode vxAddNode.cpp)
target_include_directories (vxAddNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxAddNode HipaVX_Hipacc)
create_hipacc_version(vxAddNode)
