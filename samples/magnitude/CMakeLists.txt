add_executable(magnitude magnitude.cpp)
target_include_directories(magnitude PUBLIC ${COMMON_INC_DIRS})
target_link_libraries(magnitude HipaVX_Hipacc)
create_hipacc_version(magnitude)
