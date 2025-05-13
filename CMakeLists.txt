cmake_minimum_required(VERSION 4.0)

project(NoxelEngine VERSION 1.0)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)

# cpp
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# dirs
add_subdirectory(src/libs/glad)
add_subdirectory(src/libs/glfw)
add_subdirectory(src/libs/glm)
add_subdirectory(src/engine)
add_subdirectory(src/launcher)