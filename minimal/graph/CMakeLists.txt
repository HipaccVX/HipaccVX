add_executable (simple simple.cpp)
target_include_directories (simple PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (simple HipaVX_Hipacc)

add_executable (01_simple 01_simple.cpp)
target_include_directories (01_simple PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (01_simple HipaVX_Hipacc)

add_executable (02_gen_random 02_gen_random.cpp)
target_include_directories (02_gen_random PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (02_gen_random HipaVX_Hipacc)

add_executable (03_detect_cycles 03_detect_cycles.cpp)
target_include_directories (03_detect_cycles PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (03_detect_cycles HipaVX_Hipacc)

add_executable (04_gen_acyclic 04_gen_acyclic.cpp)
target_include_directories (04_gen_acyclic PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (04_gen_acyclic HipaVX_Hipacc)

add_executable (05_reverse.cpp 05_reverse.cpp)
target_include_directories (05_reverse.cpp PUBLIC ${ROOT_DIR} ${HIPAVX_DIR})
target_link_libraries (05_reverse.cpp HipaVX_Hipacc)
