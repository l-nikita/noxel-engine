file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(engine SHARED ${sources} ${headers})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/imgui/debug_menu
    ${CMAKE_CURRENT_SOURCE_DIR}/cmd
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/assetsystem
)

target_compile_definitions(engine PRIVATE ENGINE_EXPORTS)

add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:engine>
    $<TARGET_FILE_DIR:launcher>
)

# --- libs ---- #

# ------------------
# imgui lib
file(GLOB IMGUI_SOURCES
    ../libs/imgui/*.h
    ../libs/imgui/*.cpp
    ../libs/imgui/backends/imgui_impl_sdl3.cpp
    ../libs/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ../libs/imgui
    ../libs/imgui/backends
	../libs/SDL/include 
)

# ------------------
# lodepng lib
file(GLOB LODEPNG_SOURCES
    ../libs/lodepng/lodepng.h
    ../libs/lodepng/lodepng.cpp
    #../libs/lodepng/lodepng_benchmark.cpp
    ../libs/lodepng/lodepng_fuzzer.cpp
    ../libs/lodepng/lodepng_unittest.cpp
    ../libs/lodepng/lodepng_util.cpp
    ../libs/lodepng/pngdetail.cpp
)

add_library(lodepng STATIC ${LODEPNG_SOURCES})

target_include_directories(lodepng PUBLIC
    ../libs/lodepng
)

# ------------------
# link libs
target_include_directories(engine PRIVATE 
	../libs/SDL 
	../libs/SDL/include 
	../libs/glad/include 
	../libs/glm
	../libs/imgui
	../libs/imgui/backends
)

target_link_libraries(engine PRIVATE 
	SDL3::SDL3
	glm
	imgui
	glad
	lodepng
	nlohmann_json::nlohmann_json
)