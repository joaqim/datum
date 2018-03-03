project(DATUM)
cmake_minimum_required(VERSION 3.6)

# CMake policies: don't treat imported targets with :: as files
if(POLICY CMP0028)
    cmake_policy(SET CMP0028 NEW)
endif()
# Enable MACOSX_RPATH by default
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()
# Quoted variables should not be dereferenced
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

if(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(WIN32)
	set(AR "Win32")
else()
	set(AR "x64")
endif()

#if(MSVC)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
#endif(MSVC)

set(EXECUTABLE_NAME datum)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_BIN_DIR ${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

set(CMAKE_VS_STARTUP_PROJECT ${EXECUTABLE_NAME})

set(EXTERNAL_DIR ../external)



# Set version information in a config.h file
set(DATUM_VERSION_MAJOR 1)
set(DATUM_VERSION_MINOR 0)
configure_file(
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BIN_DIR}/config.h"
	)
include_directories(${PROJECT_BIN_DIR})


if(CMAKE_COMPILER_IS_GNUCXX)
	if(XY_STATIC_RUNTIME)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -static")
	else()
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
	endif()
endif()


#SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG_")
#SET(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")

include("src/CMakeLists.txt")

if(WIN32)
	add_executable(${EXECUTABLE_NAME} WIN32 ${DATUM_SRC} )
else()
	add_executable(${EXECUTABLE_NAME} ${DATUM_SRC})
endif()

SET_TARGET_PROPERTIES(${EXECUTABLE_NAME} PROPERTIES COMPILER_FLAGS "/SUBSYSTEM:CONSOLE /MDd /std:c++17")
set_target_properties(${EXECUTABLE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_BIN_DIR})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
	CMAKE_CXX_STANDARD 17
	CMAKE_CXX_STANDARD_REQUIRED ON
	CMAKE_CXX_EXTENSIONS OFF
	CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")


function (build_external_project target prefix url) #FOLLOWING ARGUMENTS are the CMAKE_ARGS of ExternalProject_Add

	set(trigger_build_dir ${CMAKE_BINARY_DIR}/force_${target})

	#mktemp dir in build tree
	file(MAKE_DIRECTORY ${trigger_build_dir} ${trigger_build_dir}/build)

	#generate false dependency project
	set(CMAKE_LIST_CONTENT "
	cmake_minimum_required(VERSION 2.8)

	include(ExternalProject)
	ExternalProject_add(${target}
		PREFIX ${prefix}/${target}
		URL ${url}
		CMAKE_ARGS ${ARGN}
		INSTALL_COMMAND \"\"
		)

	add_custom_target(trigger_${target})
	add_dependencies(trigger_${target} ${target})")

	file(WRITE ${trigger_build_dir}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")

	execute_process(COMMAND ${CMAKE_COMMAND} ..
		WORKING_DIRECTORY ${trigger_build_dir}/build
		)
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		WORKING_DIRECTORY ${trigger_build_dir}/build
		)

endfunction()

include(ExternalProject)

#
#   Add external project.
#
#   \param name             Name of external project
#   \param path             Path to source directory
#   \param external         Name of the external target
#
macro(add_external_project name source_dir bin_dir)
	# Create external project
	#set(${name}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${path})
	#set(${name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${path})
	ExternalProject_Add(${name}
		SOURCE_DIR "${source_dir}"
		BINARY_DIR "${bin_dir}"
		CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
		"-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} /std:c++17"
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		# These are only useful if you're cross-compiling.
		# They, however, will not hurt regardless.
		"-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
		"-DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}"
		"-DCMAKE_AR=${CMAKE_AR}"
		"-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
		"-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
		"-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}"
		"-DCMAKE_COMPILER_PREFIX=${CMAKE_COMPILER_PREFIX}"
		"-DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}"
		INSTALL_COMMAND ""
		)

endmacro(add_external_project)

#
#   Add external target to external project.
#
#   \param name             Name of external project
#   \param includedir       Path to include directory
#   \param libdir           Path to library directory
#   \param build_type       Build type {STATIC, SHARED}
#   \param external         Name of the external target
#
macro(add_external_target name includedir libdir build_type external)
	# Configurations
	set(${name}_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${libdir})

	# Create external library
	add_library(${name} ${build_type} IMPORTED)
	set(${name}_LIBRARY "${${name}_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_${build_type}_LIBRARY_PREFIX}${name}${CMAKE_${build_type}_LIBRARY_SUFFIX}")

	# Find paths and set dependencies
	add_dependencies(${name} ${external})
	set(${name}_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${includedir}")

	# Set interface properties
	set_target_properties(${name} PROPERTIES IMPORTED_LOCATION ${${name}_LIBRARY})
	set_target_properties(${name} PROPERTIES INCLUDE_DIRECTORIES ${${name}_INCLUDE_DIR})
endmacro(add_external_target)

include(DownloadProject.cmake)
if (CMAKE_VERSION VERSION_LESS 3.2)
	set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
	set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

download_project(PROJ glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
	QUIET
	)

set( ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "Build basic example applications" FORCE)
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" FORCE)
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" FORCE)
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" FORCE)
set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" FORCE)
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	include_directories(${EXECUTABLE_NAME} ${OPENGL_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_gl_LIBRARY})
endif()
#add_subdirectory(${EXTERNAL_DIR}/glfw ${PROJECT_SOURCE_DIR}/glfw-external)
add_subdirectory(${glfw_SOURCE_DIR})
target_link_libraries(${EXECUTABLE_NAME} glfw ${GLFW_LIBRARIES})

include_directories(${GLFW_INCLUDE_DIRS})

#INCLUDE_EXTERNAL_MSPROJECT(${EXTERNAL_DIR}/glew-2.1.0/build/glew.sln)
#set(CMAKE_PREFIX_PATH ${EXTERNAL_DIR}/glew-2.1.0)
#set(CMAKE_LIBRARY_PATH ${EXTERNAL_DIR}/glew-2.1.0/build/Debug)

download_project(PROJ                glew
	GIT_REPOSITORY      https://github.com/nigels-com/glew.git
	GIT_TAG             master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
	QUIET
	)


add_subdirectory("${EXTERNAL_DIR}/glew-2.1.0/build/cmake" "external")
include_directories("${EXTERNAL_DIR}/glew-2.1.0/include")

if (WIN32)
	#include_directories("P:/external/glfw/include" "P:/external/glm" "P:/external/glew-2.1.0/include")
	#link_directories("P:/external/glew-2.1.0/lib/Debug/x64" "P:/external/glfw/build/src/Debug" )
else (WIN32)
	#find_package (glfw3 REQUIRED)
	#find_package (GLM REQUIRED)
	#find_package (GLEW REQUIRED STATIC)
endif (WIN32)

include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} glew ${GLEW_LIBRARIES})

if (WIN32)
	#target_link_libraries (${EXECUTABLE_NAME} ${OPENGL_LIBRARIES} glew32s glu32 opengl32)
endif (WIN32)
#endif()


download_project(PROJ                imgui
	GIT_REPOSITORY      https://github.com/ocornut/imgui.git
	GIT_TAG             master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
	QUIET
	)

include_directories(${imgui_SOURCE_DIR})

if(UNIX)
	find_package(X11 REQUIRED)

	if(X11_FOUND)
		include_directories(${X11_INCLUDE_DIRS})
		target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES})
	endif()
endif()

set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXECUTABLE_NAME})

include_directories(
	${CMAKE_SOURCE_DIR}/include)


set_target_properties(${EXECUTABLE_NAME} PROPERTIES PROJECT_BIN_DIR  "${PROJECT_BIN_DIR}")
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${PROJECT_BIN_DIR})

# CPack packaging
include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${DATUM_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${DATUM_VERSION_MINOR}")
include(CPack)

# Copy the needed dlls based the architecture
#if (NOT UNIX)
#add_custom_command(
#    TARGET ${EXECUTABLE_NAME}
#    PRE_BUILD
#        #COMMAND ${CMAKE_COMMAND} -E copy ${OF_PATH}/export/vs/${AR}/FreeImage.dll ${VORP_BIN_DIR}/
#    )
#endif(WIN32)

# This function is used to force a build on a dependant project at cmake configuration phase.
# 
