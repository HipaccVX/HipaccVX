add_executable(teaser teaser.cpp)
target_include_directories(teaser PUBLIC ${COMMON_INC_DIRS})
target_link_libraries(teaser HipaVX_Hipacc)
create_hipacc_version(teaser)
