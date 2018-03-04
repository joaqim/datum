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

set(EXECUTABLE_NAME datum)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories("${CMAKE_SOURCE_DIR}/include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

include("src/CMakeLists.txt")

if(WIN32)
	set(AR "Win32")

	add_executable(${EXECUTABLE_NAME} WIN32 ${DATUM_SRC})
	
	#SET_TARGET_PROPERTIES(${EXECUTABLE_NAME} PROPERTIES COMPILER_FLAGS "/SUBSYSTEM:CONSOLE /MDd /std:c++17")
    #set_target_properties(${EXECUTABLE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_BIN_DIR})

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

add_subdirectory(${glfw_SOURCE_DIR})
target_link_libraries(${EXECUTABLE_NAME} glfw ${GLFW_LIBRARIES})
include_directories(${GLFW_INCLUDE_DIRS})
if()
	download_project(PROJ                opencv
	GIT_REPOSITORY      https://github.com/opencv/opencv.git
	GIT_TAG             master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
	QUIET
	)
add_subdirectory(${opencv_SOURCE_DIR})

include_directories(${opencv_INCLUDE_DIRS})
include_directories(${opencv_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
#link_libraries(${OpenCV_LIBS})
endif()

#include_directories($ENV{OPENCV_DIR})

else()
	set(AR "x64")

add_executable(${EXECUTABLE_NAME} ${DATUM_SRC})


# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})


# opengl
find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
include_directories(${OPENGL_INCLUDE_DIR})
	include_directories(${EXECUTABLE_NAME} ${OPENGL_INCLUDE_DIRS})
	target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_gl_LIBRARY})
endif()
	
include(FindPkgConfig)

pkg_search_module(POPPLERCPP REQUIRED QUIET poppler-cpp)
link_directories(${POPPLERCPP_LIBRARY_DIRS})

include_directories("${POPPLER_INCLUDE_DIR}/cpp")

endif()

# OpenCV
#find_package(OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
#link_directories(${OpenCV_LIB_DIR})

include_directories($ENV{OPENCV_DIR}/include)

include_directories("D:/opencv/build/include")

download_project(PROJ poppler-data
	GIT_REPOSITORY https://anongit.freedesktop.org/git/poppler/poppler-data.git
	GIT_TAG master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
	QUIET
	)

add_subdirectory(${poppler-data_SOURCE_DIR})
include_directories(${poppler-data_INCLUDE_DIRS})

download_project(PROJ poppler
	GIT_REPOSITORY https://anongit.freedesktop.org/git/poppler/poppler.git
	GIT_TAG master
	${UPDATE_DISCONNECTED_IF_AVAILABLE}
	QUIET
	)

add_subdirectory(${poppler_SOURCE_DIR})
include_directories(${poppler_INCLUDE_DIRS})
	
# Poppler
find_package(Poppler REQUIRED)
include_directories(${POPPLER_INCLUDE_DIRS})
message("${POPPLER_INCLUDE_DIRS}")



target_link_libraries(
        ${EXECUTABLE_NAME}
        glfw
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${EXTRA_LIBS}
	${OpenCV_LIBS}
    ${POPPLER_LIBRARY}
    poppler-cpp
)

if()
add_executable(SVMTrain "src/SVMTrain.cpp")
target_link_libraries(
SVMTrain
${OpenCV_LIBS}
)

add_executable(RecogSVM "src/RecogSVM.cpp")
target_link_libraries(
RecogSVM
${OpenCV_LIBS}
)
endif()
#include("licPlaterec")
#add_executable(licPlaterec
#	${licPlaterec_SRC})