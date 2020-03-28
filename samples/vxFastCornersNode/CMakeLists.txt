add_executable (vxFastCornersNode vxFastCornersNode.cpp)
target_include_directories (vxFastCornersNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxFastCornersNode HipaVX_Hipacc)
create_hipacc_version(vxFastCornersNode)
