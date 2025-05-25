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
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
)

target_compile_definitions(engine PRIVATE ENGINE_EXPORTS)

add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:engine>
    $<TARGET_FILE_DIR:launcher>
)

# ------------------
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/*.h
    ${imgui_SOURCE_DIR}/*.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_compile_features(imgui PRIVATE cxx_std_20)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

# ------------------
target_link_libraries(engine PUBLIC 
	proto
	glad
	SDL3::SDL3
	SDL3_image::SDL3_image
	EnTT	
	glm
	Jolt
	nlohmann_json::nlohmann_json
	protobuf::libprotobuf
	GameNetworkingSockets
	imgui
)