add_executable (vxMagnitudeNode vxMagnitudeNode.cpp)
target_include_directories (vxMagnitudeNode PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (vxMagnitudeNode HipaVX_Hipacc)
create_hipacc_version(vxMagnitudeNode)
