add_executable(sobel sobel.cpp)
target_include_directories(sobel PUBLIC ${COMMON_INC_DIRS})
target_link_libraries(sobel HipaVX_Hipacc)
create_hipacc_version(sobel)
