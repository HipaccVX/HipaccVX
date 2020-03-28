add_executable (vxAndNode vxAndNode.cpp)
target_include_directories (vxAndNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxAndNode HipaVX_Hipacc)
create_hipacc_version(vxAndNode)
