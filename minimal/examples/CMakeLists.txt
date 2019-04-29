add_executable (harris harris.cpp)
target_include_directories (harris PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (harris HipaVX_Cpp)

add_executable (sobel sobel.cpp)
target_include_directories (sobel PUBLIC ${VX_DIR} ${HIPAVX_DIR})
target_link_libraries (sobel HipaVX_Cpp)
