
file(GLOB cpp_files "*.cpp")
add_library(dif SHARED ${cpp_files})
target_link_libraries(dif hdf5 hdf5_hl)
