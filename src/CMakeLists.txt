
file(GLOB cpp_files "*.cpp")
add_library(dif SHARED ${cpp_files})

add_executable(test test.cpp)
target_link_libraries(test dif)
