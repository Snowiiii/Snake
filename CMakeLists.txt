cmake_minimum_required (VERSION 3.8)

project ("Snake")

add_executable (Snake "Snake.cpp" "Snake.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Snake PROPERTY CXX_STANDARD 20)
endif()

