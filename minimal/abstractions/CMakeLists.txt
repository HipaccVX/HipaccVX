 

#add_executable (local local.cpp)
#target_include_directories (local PUBLIC ${HipaVX_SRC})
#target_link_libraries (local HipaVX_Cpp)

add_executable (map_test map_test.cpp)
target_include_directories (map_test PUBLIC ${HipaVX_SRC})
target_link_libraries (map_test HipaVX_Cpp)

add_executable (map_change_input map_change_input.cpp)
target_include_directories (map_change_input PUBLIC ${HipaVX_SRC})
target_link_libraries (map_change_input HipaVX_Cpp)

add_executable (map_sqrt map_sqrt.cpp)
target_include_directories (map_sqrt PUBLIC ${HipaVX_SRC})
target_link_libraries (map_sqrt HipaVX_Cpp)
