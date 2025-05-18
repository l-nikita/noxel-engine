cmake_minimum_required(VERSION 3.5...4.0)

project(NoxelEngine VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/bin)

# cpp
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# dirs
add_subdirectory(src/libs/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src/libs/glad)
add_subdirectory(src/libs/glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(src/libs/nlohmann_json)

add_subdirectory(src/engine)
add_subdirectory(src/launcher)