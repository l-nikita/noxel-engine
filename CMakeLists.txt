cmake_minimum_required( VERSION 3.20 )

project( NoxelEngine
        VERSION 1.0.0
        LANGUAGES C CXX
)

# ------------------ Output Directories -----------------------
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

# ------------------ C++ -----------------------
set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# ------------------ Platform Specifics ------------------
if ( MSVC )
    add_compile_options( /W4 /permissive- /utf-8 /MP )
    add_compile_definitions( UNICODE _UNICODE )
else ()
    add_compile_options( -Wall -Wextra -Wpedantic )
endif ()

# ------------------ Dependencies ------------------
find_package( SDL3 REQUIRED CONFIG )
find_package( EnTT REQUIRED CONFIG )
find_package( glm REQUIRED CONFIG )
find_package( Jolt REQUIRED CONFIG )
find_package( nlohmann_json REQUIRED CONFIG )
find_package( imgui REQUIRED CONFIG )
find_package( mimalloc REQUIRED CONFIG )
find_package( Stb REQUIRED )
find_path( TINYGLTF_INCLUDE_DIRS "tiny_gltf.h" )
#find_package(Tracy CONFIG REQUIRED)

# ------------------ Settings ------------------
set( BUILD_STATIC_LIB OFF CACHE INTERNAL "" )
set( BUILD_SHARED_LIB ON CACHE INTERNAL "" )
set( JSON_BuildTests OFF CACHE INTERNAL "" )

# ------------------ Subdirectories --------------------

add_subdirectory( src/engine )
add_subdirectory( src/launcher )

set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "launcher" )
