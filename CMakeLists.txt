CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(DeepImageFileFormat)

INCLUDE_DIRECTORIES(/usr/local/include/Field3D/)
INCLUDE_DIRECTORIES(/usr/include/OpenEXR/)

INCLUDE_DIRECTORIES(include/)


ADD_EXECUTABLE(test test.cpp)
target_link_libraries(test Field3D hdf5 hdf5_hl dl Imath Half)
