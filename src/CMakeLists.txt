FILE(GLOB DATUM_SRC "${PROJECT_SOURCE_DIR}/*.cpp" "${PROJECT_SOURCE_DIR}/*.cxx" "${PROJECT_SOURCE_DIR}/*.hpp" "${PROJECT_SOURCE_DIR}/*.inl")

if(WIN32) # if windows and not linux
	LIST(REMOVE_ITEM DATUM_SRC ${PROJECT_SOURCE_DIR}/unix_main.cpp)
else()
	LIST(REMOVE_ITEM DATUM_SRC ${PROJECT_SOURCE_DIR}/win32_main.cpp)
endif()

set(DATUM_SRC "src/test_digits.cpp")
#set(DATUM_SRC "src/SVMTrain.cpp")
