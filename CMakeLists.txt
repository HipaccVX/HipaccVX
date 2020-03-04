cmake_minimum_required(VERSION 3.7)

project(HipaVX)
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")

set (CMAKE_CXX_STANDARD 11)

set(VX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/VX/)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(HIPAVX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hipaVX)
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/kernels/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/dsl/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/graph/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})
file(GLOB GLOB_RESULT LIST_DIRECTORIES FALSE ${HIPAVX_DIR}/gen/*)
list(APPEND HipaVX_SRC ${GLOB_RESULT})

set(HIPACC_VERSION   "0.8.3"   CACHE STRING "The Hipacc version to use")


# Create the hipacc subfolder
if(NOT EXISTS "${ROOT_DIR}/hipacc" AND NOT IS_DIRECTORY "${ROOT_DIR}/hipacc")
	file(MAKE_DIRECTORY "${ROOT_DIR}/hipacc")
endif()

# Download the hipacc binary release .deb
if(NOT EXISTS "${ROOT_DIR}/hipacc/hipacc_${HIPACC_VERSION}_amd64.deb")
	file(DOWNLOAD "https://github.com/hipacc/hipacc/releases/download/v${HIPACC_VERSION}/hipacc_${HIPACC_VERSION}_amd64.deb"
		"${ROOT_DIR}/hipacc/hipacc_${HIPACC_VERSION}_amd64.deb"
		SHOW_PROGRESS
		STATUS status
		LOG log)
	list(GET status 0 status_code)
	list(GET status 1 status_string)
	
	if(NOT status_code EQUAL 0)
		message(FATAL_ERROR "error: downloading the hipacc binary package failed
			status_code: ${status_code}
			status_string: ${status_string}
			log: ${log}
			")
	endif()
endif()


# Unpach hipacc binary .deb, move to right location and delete unnecessary files
if(NOT EXISTS "${ROOT_DIR}/hipacc/bin" AND NOT IS_DIRECTORY "${ROOT_DIR}/hipacc/bin")
	execute_process(COMMAND "dpkg-deb" "-R" "${ROOT_DIR}/hipacc/hipacc_${HIPACC_VERSION}_amd64.deb" "${ROOT_DIR}/hipacc/"
		RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "error: couldn't unpack the .deb package")
	endif()
	execute_process(COMMAND "mv" "${ROOT_DIR}/hipacc/usr/local/hipacc-${HIPACC_VERSION}/bin" "${ROOT_DIR}/hipacc/"
		RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "error: couldn't move the bin folder of hipacc")
	endif()
	execute_process(COMMAND "mv" "${ROOT_DIR}/hipacc/usr/local/hipacc-${HIPACC_VERSION}/include" "${ROOT_DIR}/hipacc/"
		RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "error: couldn't move the include folder of hipacc")
	endif()
	execute_process(COMMAND "mv" "${ROOT_DIR}/hipacc/usr/local/hipacc-${HIPACC_VERSION}/lib" "${ROOT_DIR}/hipacc/"
		RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "error: couldn't move the lib folder of hipacc")
	endif()
	execute_process(COMMAND "mv" "${ROOT_DIR}/hipacc/usr/local/hipacc-${HIPACC_VERSION}/samples" "${ROOT_DIR}/hipacc/"
		RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "error: couldn't move the samples folder of hipacc")
	endif()
	execute_process(COMMAND "rm" "-r" "${ROOT_DIR}/hipacc/DEBIAN" "${ROOT_DIR}/hipacc/usr"
		RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		message(FATAL_ERROR "error: couldn't delete the unpacked folder in hipacc/")
	endif()

	# Patch wrong __local.h line in ?old? versions
	execute_process(COMMAND "sed" "-i"
					"s,#elif (defined(__GLIBC__) || defined(__APPLE__)      || defined(__FreeBSD__) \\\\,#elif (defined(__APPLE__)      || defined(__FreeBSD__) \\\\,g"
					"${ROOT_DIR}/hipacc/include/c++/v1/__locale")
endif()


set(COMMON_INC_DIRS ${VX_DIR} ${HIPAVX_DIR} ${ROOT_DIR})

file(GLOB_RECURSE HipaccGen_SRC ${HIPAVX_DIR}/gen/hipacc_*)

get_filename_component(DEFAULT_HIPACC_KERNELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hipacc_kernels ABSOLUTE)
set(HIPACC_KERNEL_DIR ${DEFAULT_HIPACC_KERNELS_DIR} CACHE STRING "Directory of the Hipacc Kernels")
add_definitions(-DHIPACC_KERNEL_DIR=\"${HIPACC_KERNEL_DIR}\")
get_filename_component(DEFAULT_CPP_KERNELS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpp_kernels ABSOLUTE)
set(CPP_KERNEL_DIR ${DEFAULT_CPP_KERNELS_DIR} CACHE STRING "Directory of the Cpp Kernels")
add_definitions(-DCPP_KERNEL_DIR=\"${CPP_KERNEL_DIR}\")

add_library (HipaVX_Hipacc ${HipaVX_SRC} ${HipaccGen_SRC})
add_library(hipaccRuntime UNKNOWN IMPORTED)
set_target_properties(hipaccRuntime PROPERTIES IMPORTED_LOCATION ${ROOT_DIR}/hipacc/lib/libhipaccRuntime.a)

find_package( OpenCV REQUIRED )

function(create_hipacc_version target_name)
	set("${target_name}_out_filename" "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_hipacc.cpp")
	set("${target_name}_out_hipacc_cpu_filename" "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_hipacc_cpu.cpp")

	target_compile_definitions(${target_name} PRIVATE HIPAVX_OUTPUT_FILENAME="${${target_name}_out_filename}")
	add_custom_command(	
		OUTPUT "${${target_name}_out_filename}"
		COMMAND ${target_name}
		DEPENDS ${target_name}
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	add_custom_command(
		OUTPUT "${${target_name}_out_hipacc_cpu_filename}"
		COMMAND "${ROOT_DIR}/hipacc/bin/hipacc" "-emit-cpu" "-std=c++11" "-nostdinc++"
												"-I${ROOT_DIR}/hipacc/include/c++/v1"
												"-I${ROOT_DIR}/hipacc/include/clang"
												"-I${ROOT_DIR}/hipacc/include/dsl"
												"-I${ROOT_DIR}"
												"${${target_name}_out_filename}"
												"-o" "${${target_name}_out_hipacc_cpu_filename}"
		DEPENDS "${${target_name}_out_filename}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
  
  add_executable (hipacc_${target_name} "${${target_name}_out_hipacc_cpu_filename}")
  target_compile_options(hipacc_${target_name} PRIVATE -std=c++11 -O2)
  target_include_directories (hipacc_${target_name} PUBLIC "${ROOT_DIR}/hipacc/include")
  #target_link_directories (hipacc_${target_name} PUBLIC "${ROOT_DIR}/hipacc/lib")
  #target_link_libraries (hipacc_${target_name} hipaccRuntime ${OpenCV_LIBS})
  target_link_libraries(hipacc_${target_name} hipaccRuntime ${OpenCV_LIBS})

	if(OpenCV_VERSION VERSION_LESS "4.0")
		target_compile_definitions(hipacc_${target_name} PRIVATE USE_OPENCV)
	else()
		target_compile_definitions(hipacc_${target_name} PRIVATE USE_OPENCV USE_OPENCV4)
	endif()
endfunction(create_hipacc_version)

#add_executable (main_hipacc main.cpp)
#target_include_directories (main_hipacc PUBLIC ${VX_DIR} ${HIPAVX_DIR})
#target_link_libraries (main_hipacc HipaVX_Hipacc)

add_subdirectory(tests)
add_subdirectory(samples)
