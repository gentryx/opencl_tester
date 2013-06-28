cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
find_package(Boost REQUIRED program_options system thread)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(opencl_tester src/main.cpp)
target_link_libraries(opencl_tester ${Boost_LIBRARIES} OpenCL)
