include_directories(${ROOT_DIR})

add_executable (test_all test_all.cpp)
target_include_directories (test_all PUBLIC ${COMMON_INC_DIRS})
target_link_libraries (test_all HipaVX_Hipacc)

get_directory_property(targets BUILDSYSTEM_TARGETS)
foreach(TARGET_DEFINED ${targets})
	get_target_property(tmp ${TARGET_DEFINED} NAME)
	create_hipacc_version(${tmp})
endforeach()
