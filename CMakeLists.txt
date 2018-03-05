project(DATUM)
cmake_minimum_required(VERSION 3.6)
set (CMAKE_CXX_STANDARD 17)

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
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/../external)

include("src/CMakeLists.txt")

if(WIN32)
	set(AR "Win32")

	add_executable(${EXECUTABLE_NAME} WIN32 ${DATUM_SRC})
else()
	set(AR "x64")

add_executable(${EXECUTABLE_NAME} ${DATUM_SRC})
endif()

# imgui
include_directories(${EXTERNAL_DIR}/imgui)
file(GLOB imgui_SRC ${EXTERNAL_DIR}/imgui/*.cpp)
add_library(imgui ${imgui_SRC})

# glfw
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Poppler
#find_package(Poppler REQUIRED)
#include_directories(${POPPLER_INCLUDE_DIRS})
#message("${POPPLER_INCLUDE_DIRS}")
#include_directories("${POPPLER_INCLUDE_DIR}/cpp")

include(FindPkgConfig)

pkg_search_module(POPPLERCPP REQUIRED QUIET poppler-cpp)
link_directories(${POPPLERCPP_LIBRARY_DIRS})

include_directories("${POPPLERCPP_INCLUDE_DIRS}")

target_link_libraries(
        ${EXECUTABLE_NAME}
        glfw
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${EXTRA_LIBS}
	${OpenCV_LIBS}
    ${POPPLER_LIBRARY}
    poppler-cpp
    ${POPPLER_LIBRARY}
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

include("src/licPlateRec/CMakeLists.txt")
#add_subdirectory("src/licPlateRec")

add_executable(licPlateRec ${licPlateRec_SRC})
target_link_libraries(licPlateRec
	${OpenCV_LIBS}
	imgui
    ${POPPLER_LIBRARY}
    poppler-cpp
 
    glfw
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${EXTRA_LIBS}

)
