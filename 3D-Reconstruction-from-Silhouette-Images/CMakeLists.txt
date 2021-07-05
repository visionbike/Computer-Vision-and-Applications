cmake_minimum_required(VERSION 3.8)
project(3D_Reconstruction_from_Silhouette_Images)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp voxel.h voxel.cpp camera.h camera.cpp reconstructor_3d.h reconstructor_3d.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})