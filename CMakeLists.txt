cmake_minimum_required (VERSION 3.15)

project (Snake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable (Snake src/Snake.cpp)

target_link_libraries(Snake ${OpenCV_LIBS})

