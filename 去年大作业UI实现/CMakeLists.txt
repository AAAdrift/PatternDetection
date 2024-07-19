cmake_minimum_required (VERSION 3.10)

set(CMAKE_BUILD_TYPE "Debug")

project(task_master)
add_executable(task_master main.cpp modify.cpp inquiry.cpp Auxiliary_function.cpp reminder.cpp task_manager.h)
include_directories(${sfml-graphics})
include_directories(${sfml-window})
include_directories(${sfml-system})
include_directories(${pthread})

target_link_libraries(task_master sfml-graphics)
target_link_libraries(task_master sfml-window)
target_link_libraries(task_master sfml-system)
target_link_libraries(task_master pthread)
