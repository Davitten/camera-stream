cmake_minimum_required(VERSION 3.11)
project(carlo)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")

# Not possible as a submodule due to linked issue:
# https://github.com/opencv/opencv/issues/20548

# Installation steps:
# https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
find_package(OpenCV REQUIRED)
message(STATUS "Found OpenCV at ${OpenCV_DIR}")


# Installation guide:
# https://github.com/realsenseai/librealsense/blob/master/doc/installation.md
find_package(realsense2 REQUIRED)
message(STATUS "Found realsense2 at ${realsense2_DIR}")

add_subdirectory(cmake)

add_executable(carlo main.cpp)
target_link_libraries(carlo PRIVATE ${OpenCV_LIBS}  realsense2::realsense2 nadjieb_mjpeg_streamer::nadjieb_mjpeg_streamer)