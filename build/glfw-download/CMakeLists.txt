# Distributed under the OSI-approved MIT License.  See accompanying
# file LICENSE or https://github.com/Crascit/DownloadProject for details.

cmake_minimum_required(VERSION 2.8.2)

project(glfw-download NONE)

include(ExternalProject)
ExternalProject_Add(glfw-download
                    GIT_REPOSITORY;https://github.com/glfw/glfw.git;GIT_TAG;master;UPDATE_DISCONNECTED 1
                    SOURCE_DIR          "P:/datum/build/glfw-src"
                    BINARY_DIR          "P:/datum/build/glfw-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
)
