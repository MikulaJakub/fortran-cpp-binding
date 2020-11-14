cmake_minimum_required(VERSION 3.5.1)

if(NOT WIN32)
	string(ASCII 27 Esc)
	set(ColourReset "${Esc}[m")
	set(BoldRed     "${Esc}[1;31m")
	set(BoldGreen   "${Esc}[1;32m")
endif()

# set project name
set(PROJECT_NAME "fortran-cpp")
project(${PROJECT_NAME} C CXX Fortran)

# set C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
cmake_policy(SET CMP0071 NEW)

# set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_Fortran_FLAGS "-Wall -O3")

get_filename_component(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
message(STATUS "Project root folder:" ${PROJECT_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# find external packages
# find VTK libraries
find_package(VTK REQUIRED)
if (VTK_FOUND)
	MESSAGE(STATUS "${BoldGreen}Found VTK: ${VTK_USE_FILE} ${ColourReset}")
else()
	MESSAGE(FATAL_ERROR "${BoldRed}VTK libraries not found !${BoldRed}")
endif()

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${HEADER_DIR})
include_directories(".")

subdirs(src)

