cmake_minimum_required(VERSION 3.5...4.0)

set(CMAKE_TOOLCHAIN_FILE "C:/FILES/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
project(NoxelEngine VERSION 1.0)

# Common
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

# Debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/lib/Debug)

# Release
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/lib/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/lib/Release)

# ------------------ C++ -----------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ------------------ Packages ------------------
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
#find_package(Tracy CONFIG REQUIRED)

# ------------------ Settings ------------------
set(BUILD_STATIC_LIB OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIB ON CACHE INTERNAL "")
set(JSON_BuildTests OFF CACHE INTERNAL "")

# ----------------------------------------------
function(auto_source_group_by_folder FILES IS_HEADER)
    foreach(file ${${FILES}})
        get_filename_component(dir "${file}" PATH)
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${dir}")

        string(REPLACE "/" ";" parts "${rel_path}")
        set(group_parts "")
		
        foreach(part ${parts})
            list(APPEND group_parts "${part}")
        endforeach()

        if(IS_HEADER)
            list(APPEND group_parts "headers")
        endif()

        string(REPLACE ";" "\\" group "${group_parts}")

        source_group("${group}" FILES "${file}")
    endforeach()
endfunction()

# ------------------ Main projects -------------
add_subdirectory(src/proto)
add_subdirectory(src/engine)
add_subdirectory(src/launcher)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "launcher")