CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(DeepImageFileFormat)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -I${PROJECT_SOURCE_DIR}/include -ggdb")
ADD_SUBDIRECTORY(src)

ADD_EXECUTABLE(test test.cpp)
target_link_libraries(test dif hdf5 hdf5_hl)
