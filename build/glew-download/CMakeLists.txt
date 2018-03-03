# Distributed under the OSI-approved MIT License.  See accompanying
# file LICENSE or https://github.com/Crascit/DownloadProject for details.

cmake_minimum_required(VERSION 2.8.2)

project(glew-download NONE)

include(ExternalProject)
ExternalProject_Add(glew-download
                    GIT_REPOSITORY;https://github.com/nigels-com/glew.git;GIT_TAG;master;UPDATE_DISCONNECTED 1
                    SOURCE_DIR          "P:/datum/build/glew-src"
                    BINARY_DIR          "P:/datum/build/glew-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
)
