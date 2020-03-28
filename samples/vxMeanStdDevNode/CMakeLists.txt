add_executable (vxMeanStdDevNode vxMeanStdDevNode.cpp)
target_include_directories (vxMeanStdDevNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxMeanStdDevNode HipaVX_Hipacc)
create_hipacc_version(vxMeanStdDevNode)
