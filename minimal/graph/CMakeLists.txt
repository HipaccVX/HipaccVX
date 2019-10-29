add_executable (01_simple 01_simple.cpp)
target_include_directories (01_simple PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (01_simple HipaVX_Cpp)

add_executable (02_gen_random 02_gen_random.cpp)
target_include_directories (02_gen_random PUBLIC ${HipaVX_Cpp} ${HIPAVX_DIR})
target_link_libraries (02_gen_random HipaVX_Cpp)

add_executable (03_detect_cycles 03_detect_cycles.cpp)
target_include_directories (03_detect_cycles PUBLIC ${HipaVX_Cpp} ${HIPAVX_DIR})
target_link_libraries (03_detect_cycles HipaVX_Cpp)

add_executable (04_gen_acyclic 04_gen_acyclic.cpp)
target_include_directories (04_gen_acyclic PUBLIC ${HipaVX_Cpp} ${HIPAVX_DIR})
target_link_libraries (04_gen_acyclic HipaVX_Cpp)

add_executable (05_reverse 05_reverse.cpp)
target_include_directories (05_reverse PUBLIC ${HipaVX_Cpp} ${HIPAVX_DIR})
target_link_libraries (05_reverse HipaVX_Cpp)

add_executable (06_dead_node_elimination 06_dead_node_elimination.cpp)
target_include_directories (06_dead_node_elimination PUBLIC ${HipaVX_Cpp} ${HIPAVX_DIR})
target_link_libraries (06_dead_node_elimination HipaVX_Cpp)

add_executable (07_topological_sort 07_topological_sort.cpp)
target_include_directories (07_topological_sort PUBLIC ${HipaVX_Cpp} ${HIPAVX_DIR})
target_link_libraries (07_topological_sort HipaVX_Cpp)
