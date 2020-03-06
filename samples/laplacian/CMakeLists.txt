add_executable(laplacian laplacian.cpp)
target_include_directories(laplacian PUBLIC ${COMMON_INC_DIRS})
target_link_libraries(laplacian HipaVX_Hipacc)
create_hipacc_version(laplacian)
