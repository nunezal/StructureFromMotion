cmake_minimum_required(VERSION 3.10)
project(StructureFromMotion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
# Add Homebrew OpenCV path for macOS
if(APPLE)
    # Check for Apple Silicon (M1/M2/etc) with opencv in /opt/homebrew
    if(EXISTS "/opt/homebrew/Cellar/opencv")
        set(OpenCV_DIR "/opt/homebrew/Cellar/opencv/4.11.0_1/lib/cmake/opencv4" CACHE PATH "OpenCV directory")
        message(STATUS "Using Apple Silicon OpenCV path")
    # Check for Intel Mac with opencv in /usr/local
    elseif(EXISTS "/usr/local/Cellar/opencv")
        set(OpenCV_DIR "/usr/local/Cellar/opencv/4.11.0_1/lib/cmake/opencv4" CACHE PATH "OpenCV directory")
        message(STATUS "Using Intel Mac OpenCV path")
    endif()
    
    # If specific path doesn't exist, try to find it
    if(NOT EXISTS ${OpenCV_DIR})
        execute_process(
            COMMAND find /opt/homebrew/Cellar /usr/local/Cellar -name "OpenCVConfig.cmake" -o -name "opencv4Config.cmake" 2>/dev/null
            OUTPUT_VARIABLE OPENCV_CMAKE_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(OPENCV_CMAKE_PATH)
            get_filename_component(OpenCV_DIR "${OPENCV_CMAKE_PATH}" DIRECTORY)
            message(STATUS "Found OpenCV at: ${OpenCV_DIR}")
        endif()
    endif()
endif()

find_package(OpenCV REQUIRED)
find_package(COLMAP QUIET)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})
if(COLMAP_FOUND)
    include_directories(${COLMAP_INCLUDE_DIRS})
endif()

# Print debug info
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

# Add executable
add_executable(video_to_3d 
    src/main.cpp
    src/frame_extractor.cpp
)

# Link libraries
target_link_libraries(video_to_3d ${OpenCV_LIBS})
if(COLMAP_FOUND)
    target_link_libraries(video_to_3d ${COLMAP_LIBRARIES})
endif() 