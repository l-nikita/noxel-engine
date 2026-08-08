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

add_custom_target( copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Updating runtime assets"
)

add_dependencies( launcher copy_assets )
