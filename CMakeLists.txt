cmake_minimum_required(VERSION 3.2)
project(computer-science-interview
        VERSION 0.1
        DESCRIPTION "Preparing for computer science interview(C/C++)"
        HOMEPAGE_URL "https://github.com/javioustlj/cs-interview"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)


# set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -Werror ${CMAKE_CXX_FLAGS} ")
# set(CMAKE_C_FLAGS "-Wall -pedantic -Wextra -Werror ${CMAKE_CXX_FLAGS} ")

set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra ${CMAKE_CXX_FLAGS} ")
set(CMAKE_C_FLAGS "-Wall -pedantic -Wextra ${CMAKE_CXX_FLAGS} ")

# set(INSTALL_BIN_DIR "${CMAKE_SOURCE_DIR}/bin")
# set(INSTALL_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

# set(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
# set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)


