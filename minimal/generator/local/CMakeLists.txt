
add_executable (localop_at_and_reduce localop_at_and_reduce.cpp)
target_include_directories (localop_at_and_reduce PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (localop_at_and_reduce HipaVX_Cpp)

add_executable (localop_forall localop_forall.cpp)
target_include_directories (localop_forall PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (localop_forall HipaVX_Cpp)

add_executable (localop_local_to_pixel localop_local_to_pixel.cpp)
target_include_directories (localop_local_to_pixel PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (localop_local_to_pixel HipaVX_Cpp)

add_executable (localop_chained_all localop_chained_all.cpp)
target_include_directories (localop_chained_all PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (localop_chained_all HipaVX_Cpp)

