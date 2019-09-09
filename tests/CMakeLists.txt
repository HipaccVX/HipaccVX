include_directories(${ROOT_DIR})

add_executable (gaussian gaussian.cpp)
target_include_directories (gaussian PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (gaussian HipaVX_Hipacc)
add_test("gaussian" gaussian)

add_executable (magnitude magnitude.cpp)
target_include_directories (magnitude PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (magnitude HipaVX_Hipacc)
add_test("magnitude" magnitude)

add_executable (multiply_node multiply_node.cpp)
target_include_directories (multiply_node PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (multiply_node HipaVX_Hipacc)
add_test("multiply_node" multiply_node)
