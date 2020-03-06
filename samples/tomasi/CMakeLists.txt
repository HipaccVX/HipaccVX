add_executable(tomasi tomasi.cpp)
target_include_directories(tomasi PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (tomasi HipaVX_Hipacc)
create_hipacc_version(tomasi)
