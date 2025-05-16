add_executable(launcher)

target_sources(launcher PRIVATE
    main.cpp
)

# --- libs ---- #
target_link_libraries(launcher PRIVATE engine)

target_include_directories(launcher PRIVATE
	../engine
)

target_include_directories(launcher PRIVATE 
	../libs/glad/include 
	../libs/glfw/include 
	../libs/glm
	../libs/imgui
	../libs/imgui/backends
)

add_custom_command(TARGET launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:engine>
    $<TARGET_FILE_DIR:launcher>
)