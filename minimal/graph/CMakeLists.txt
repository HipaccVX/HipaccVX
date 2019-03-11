add_executable (simple simple.cpp)
target_include_directories (simple PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (simple HipaVX_Hipacc)

add_executable (01_simple 01_simple.cpp)
target_include_directories (01_simple PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (01_simple HipaVX_Hipacc)
