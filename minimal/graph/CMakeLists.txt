add_executable (simple simple.cpp)
target_include_directories (simple PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (simple HipaVX_Hipacc)
