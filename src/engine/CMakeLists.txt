file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(engine SHARED ${sources} ${headers})

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#add_library(engine SHARED)

# file(GLOB_RECURSE UTILS_SOURCE 
	# ./utils/*.cpp
	# ./utils/*.hpp
# )

# file(GLOB_RECURSE GUI_SOURCE 
	# ./gui/*.cpp
	# ./gui/*.hpp
	# ./gui/imgui/*.cpp
	# ./gui/imgui/*.hpp
	# ./gui/imgui/debug_menu/*.cpp
	# ./gui/imgui/debug_menu/*.hpp
# )

# source_group("GUI Files" FILES ${GUI_SOURCE})
# source_group("Utils Files" FILES ${UTILS_SOURCE})

# target_sources(engine PRIVATE
    # engine.cpp
    # engine.hpp
	# cmd.cpp
	# cmd.hpp
	# ${GUI_SOURCE}
	# ${UTILS_SOURCE}
# )

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
    ../libs/imgui/backends/imgui_impl_glfw.cpp
    ../libs/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ../libs/imgui
    ../libs/imgui/backends
	../libs/glfw/include
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
	../libs/glad/include 
	../libs/glfw/include 
	../libs/glm
	../libs/imgui
	../libs/imgui/backends
)

target_link_libraries(engine PRIVATE 
	glfw
	glm
	imgui
	lodepng
	glad
)