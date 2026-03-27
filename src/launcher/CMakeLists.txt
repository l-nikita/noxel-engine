add_executable(launcher)

target_sources(launcher PRIVATE
    main.cpp
)

# --- libs ---- #
target_link_libraries(launcher PRIVATE engine)
target_include_directories(launcher PRIVATE
	../engine
)

add_custom_command(TARGET launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:engine>
    $<TARGET_FILE_DIR:launcher>
)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(ASSETS_BINARY_DIR "$<TARGET_FILE_DIR:launcher>/assets")

add_custom_command(TARGET launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_SOURCE_DIR}"
            "${ASSETS_BINARY_DIR}"
    COMMENT "Copying assets to build directory..."
)

set_target_properties(launcher PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)