cmake_minimum_required(VERSION 3.5...4.0)

set(CMAKE_TOOLCHAIN_FILE "E:/DEV/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
project(NoxelEngine VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/bin)

# cpp
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# libs
find_package(Protobuf REQUIRED)

# dirs
add_subdirectory(src/libs/SDL EXCLUDE_FROM_ALL)
add_subdirectory(src/libs/glad)
add_subdirectory(src/libs/glm)
add_subdirectory(src/libs/JoltPhysics/Build)

set(BUILD_SHARED_LIB OFF CACHE INTERNAL "")
add_subdirectory(src/libs/GameNetworkingSockets)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(src/libs/nlohmann_json)

add_subdirectory(src/libs/entt)

add_subdirectory(src/proto)
add_subdirectory(src/engine)
add_subdirectory(src/launcher)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "launcher")