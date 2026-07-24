add_executable( launcher main.cpp )

# --- libs ---- #
target_link_libraries( launcher PRIVATE
        mimalloc
        engine
)

set_target_properties( launcher PROPERTIES
        OUTPUT_NAME "noxel-engine"
        BUILD_RPATH "${CMAKE_BINARY_DIR}/src/engine"
        INSTALL_RPATH "${CMAKE_BINARY_DIR}/src/engine"
)

set( ASSETS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/assets" )
set( ASSETS_BINARY_DIR "${CMAKE_BINARY_DIR}/assets" )

add_custom_command( TARGET launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSETS_SOURCE_DIR}"
        "${ASSETS_BINARY_DIR}"
        COMMENT "Copying assets from ${ASSETS_SOURCE_DIR} to ${ASSETS_BINARY_DIR}"
)
