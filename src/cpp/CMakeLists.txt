cmake_minimum_required(VERSION 3.2)
project(computer-science-interview-cpp
        LANGUAGES CXX
        VERSION 0.1)

add_executable(sizeof sizeof.cpp)
add_executable(unsigned unsigned.cpp)
add_executable(pointer pointer.cpp)
add_executable(polymorphism polymorphism.cpp)
add_executable(lambda lambda.cpp)
add_executable(smart_pointer smart_pointer.cpp)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        add_executable(abrt_debug debug/unwind.cpp)
        add_executable(unwind unwinding.cpp)
endif()
