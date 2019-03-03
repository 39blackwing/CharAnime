# Cmake mininum version
cmake_minimum_required (VERSION 3.10)

# OS info
MESSAGE("Identifying the OS...")
if(WIN32)
  MESSAGE("This is Windows.")
  ADD_DEFINITIONS(-D _OS_WIN)
elseif(UNIX)
  MESSAGE("This is Unix.")
  ADD_DEFINITIONS(-D _OS_UNIX)
elseif(APPLE)
  MESSAGE("This is MacOS.")
  ADD_DEFINITIONS(-D _OS_MAC)
endif()

# Get folder name ${FOLDER_NAME}
STRING(REGEX REPLACE ".*/(.*)" "\\1" FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR})

# Project name ${CMAKE_PROJECT_NAME}
project (${FOLDER_NAME})

# Debug Version (Opinions: Release Debug MinSizeRel RelWithDebInfo)
set (CMAKE_BUILD_TYPE "Debug")

# Debug opinions
set (CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -DDEBUG -g2 -ggdb")

# Release opinion
set (CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Add std c++11
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Add include dir
include_directories(dev/ffmpeg/include)

# Add lib dir
link_directories(./dev/ffmpeg/lib)

# Add src dir
aux_source_directory (src DIR_SRCS)

# Add executable output path
set (EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

# Add executable
add_executable (${CMAKE_PROJECT_NAME} ${DIR_SRCS})


# Link libraries
target_link_libraries (${CMAKE_PROJECT_NAME}
                      pthread
                      avutil
                      avformat
                      avcodec
                      swscale
                      )
