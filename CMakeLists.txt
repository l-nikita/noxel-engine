cmake_minimum_required(VERSION 3.5...4.0)

set(CMAKE_TOOLCHAIN_FILE "E:/DEV/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
project(NoxelEngine VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/bin)

# ------------------ cpp ------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ------------------ packages ------------------
find_package(SDL3 REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(Jolt REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)
find_package(Protobuf REQUIRED CONFIG)
find_package(GameNetworkingSockets REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(Stb REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(src/libs/glad)

# ------------------ settings ------------------
set(BUILD_STATIC_LIB OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIB ON CACHE INTERNAL "")
set(JSON_BuildTests OFF CACHE INTERNAL "")

# ------------------ main projects ------------------
add_subdirectory(src/proto)
add_subdirectory(src/engine)
add_subdirectory(src/launcher)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "launcher")