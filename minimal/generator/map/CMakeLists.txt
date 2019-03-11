
add_executable (map_simple map_simple.cpp)
target_include_directories (map_simple PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (map_simple HipaVX_Cpp)

add_executable (map_change_in_out map_change_in_out.cpp)
target_include_directories (map_change_in_out PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (map_change_in_out HipaVX_Cpp)
