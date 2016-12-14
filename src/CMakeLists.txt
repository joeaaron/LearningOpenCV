#######################################################
# Copyright:   Shanghai ReCAT Control Tech. Co., Ltd.
# Author:      ECAT Vision Team
# Version:     1.0
# Data:        2016.12.14
# url:         http://www.ecatmc.cc
########################################################
cmake_minimum_required (VERSION 2.6) 

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")

find_package(OpenCV 3.0.0 REQUIRED opencv_world)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/common" commonsrc)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
source_group("Include" FILES ${CURRENT_HEADERS})

add_executable(findsquares squares.cpp ${CURRENT_HEADERS} ${commonsrc})
target_link_libraries(findsquares ${OpenCV_LIBS})
set_target_properties(findsquares PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

