# Open The CMake GUI
# specify the source directory and the binary directory
# press configure
# set CMAKE_INSTALL_PREFIX to the path where you want to install the program
# press configure
# check BUILD_WITH_QT if you want to build the program with a QT-Config GUI
# check GENERATE_DEB_PACKAGE if you want to build a debian package (only on Linux)
#
# UNIX Makefile:
# 1) go to the binary folder and type "make" to build the project
# 2) (optional) type "make install all" to install the files into the install
#    directory
#
# Microsoft Visual C++:
# 1) open the .sln file
# 2) change the mode to "Release" -> only Release is supported!
# 3) build the project "ALL_BUILD" to build the Neuro-Endo-Trainer-Activity-Tracker project
# 4) build the project "INSTALL" to install the files into the install
#    directory
# 5) build the project "PACKAGE" to create an NSIS-installer (NSIS is required)


project(Neuro-Endo-Trainer-Activity-Tracker)

cmake_minimum_required(VERSION 2.6)

find_package(OpenCV REQUIRED)


#-------------------------------------------------------------------------------
#version
set(Neuro-Endo-Trainer-Activity-Tracker_MAJOR 1)
set(Neuro-Endo-Trainer-Activity-Tracker_MINOR 0)
set(Neuro-Endo-Trainer-Activity-Tracker_PATCH 0)
set(Neuro-Endo-Trainer-Activity-Tracker ${Neuro-Endo-Trainer-Activity-Tracker_MAJOR}.${Neuro-Endo-Trainer-Activity-Tracker_MINOR}.${Neuro-Endo-Trainer-Activity-Tracker_PATCH})

#------------------------------------------------------------------------------
#build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

option(WITH_OPENMP "Use OpenMP." OFF)

if(WITH_OPENMP)
	find_package(OpenMP REQUIRED)

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(WITH_OPENMP)


if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(NOT MSVC)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(src/Neuro-Endo-Trainer-Activity-Tracker)
add_subdirectory(src/libopentld)
add_subdirectory(src/3rdparty/cvblobs)

configure_file("${PROJECT_SOURCE_DIR}/OpenTLDConfig.cmake.in" "${PROJECT_BINARY_DIR}/OpenTLDConfig.cmake" @ONLY)
