################################################################################
# Project:  Lib JBIG
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, dmitry.baryshnikov@nexgis.com
################################################################################
# Copyright (C) 2015-2018, NextGIS <info@nextgis.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################

cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

project(jbig
    VERSION 2.1.0
)

if(NOT DEFINED PACKAGE_VENDOR)
    set(PACKAGE_VENDOR NextGIS)
endif()

if(NOT DEFINED PACKAGE_BUGREPORT)
    set(PACKAGE_BUGREPORT info@nextgis.com)
endif()

# Some init settings
set(CMAKE_COLOR_MAKEFILE ON)
# Set path to additional CMake modules

if(OSX_FRAMEWORK AND (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS))
  message(FATAL_ERROR "Only OSX_FRAMEWORK key or any or both BUILD_SHARED_LIBS
                       and BUILD_STATIC_LIBS keys are permitted")
endif()

if(CMAKE_GENERATOR_TOOLSET MATCHES "v([0-9]+)_xp")
    add_definitions(-D_WIN32_WINNT=0x0501)
endif()

set(PACKAGE_NAME ${PROJECT_NAME})

include(${CMAKE_CURRENT_LIST_DIR}/cmake/borsch/cmake/version.cmake)
write_version(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/CHANGES ${PROJECT_VERSION_MAJOR} ${PROJECT_VERSION_MINOR} ${PROJECT_VERSION_PATCH})
report_version(${PROJECT_NAME} ${PROJECT_VERSION})

if(OSX_FRAMEWORK)
    set(FRAMEWORK_VERSION "${JBIG_MAJOR_VERSION}")
endif()

set(PUBLIC_HDRS
    libjbig/jbig_ar.h
    libjbig/jbig85.h
    libjbig/jbig.h
)

set(DOCS
    COPYING
    CHANGES
)

set(SRCS
    libjbig/jbig.c
    libjbig/jbig85.c
    libjbig/jbig_ar.c
)

set(LIB_NAME ${PROJECT_NAME})
if(BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${SRCS} ${PUBLIC_HDRS})
    set_target_properties (${LIB_NAME} PROPERTIES
        VERSION ${VERSION}
        SOVERSION ${JBIG_MAJOR_VERSION}
        DEFINE_SYMBOL JBIG_DLL_EXPORTS)
elseif(OSX_FRAMEWORK)
    add_library(${LIB_NAME} SHARED ${SRCS} ${PUBLIC_HDRS})
    set_target_properties(${LIB_NAME} PROPERTIES
      FRAMEWORK TRUE
      FRAMEWORK_VERSION ${FRAMEWORK_VERSION}
      MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION}
      MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION}
      MACOSX_FRAMEWORK_IDENTIFIER org.jbigkit.jbig
      XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
      PUBLIC_HEADER "${PUBLIC_HDRS}"
      VERSION ${VERSION}
      SOVERSION ${JBIG_MAJOR_VERSION}
      )
else()
    add_definitions(-DJBIG_STATIC)
    add_library(${LIB_NAME} STATIC ${SRCS} ${PUBLIC_HDRS})
    #set_target_properties(${LIB_NAME} PROPERTIES DEFINE_SYMBOL JBIG_STATIC)
endif()

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O -W -Wall -ansi -pedantic")
elseif(WIN32)
    set_target_properties(${LIB_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )

if(UNIX)
    # On unix-like platforms the library is almost always called lib${PROJECT_NAME}
    set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
endif()

# Testing ======================================================================
enable_testing()
if(BUILD_TESTING)
    add_executable(tstcodec libjbig/tstcodec.c)
    target_link_libraries(tstcodec ${LIB_NAME})
    add_test(NAME tstcodec COMMAND $<TARGET_FILE:tstcodec>)

    add_executable(tstcodec85 libjbig/tstcodec85.c)
    target_link_libraries(tstcodec85 ${LIB_NAME})
    add_test(NAME tstcodec85 COMMAND $<TARGET_FILE:tstcodec85>)
endif()

# Installing and packaging
include(${CMAKE_CURRENT_LIST_DIR}/cmake/borsch/cmake/packaging.cmake)
create_borsch_package(${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    TARGETS ${LIB_NAME}
    HEADERS ${PUBLIC_HDRS}
)
