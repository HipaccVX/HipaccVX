include_directories(${ROOT_DIR})

add_executable (test_all test_all.cpp)
target_include_directories (test_all PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (test_all HipaVX_Hipacc)
add_test("test_all" test_all)
