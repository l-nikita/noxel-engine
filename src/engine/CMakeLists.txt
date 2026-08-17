file( GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "assetsystem/*.cpp"
        "client/*.cpp"
        "common/*.cpp"
        "dbg/*.cpp"
        "engine/*.cpp"
        "filesystem/*.cpp"
        #"gui/*.cpp"
        "input/*.cpp"
        "networking/*.cpp"
        "physics/*.cpp"
        "server/*.cpp"
)

file( GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        "assetsystem/*.hpp"
        "client/*.hpp"
        "common/*.hpp"
        "dbg/*.hpp"
        "engine/*.hpp"
        "filesystem/*.hpp"
        #"gui/*.hpp"
        "input/*.hpp"
        "networking/*.hpp"
        "physics/*.hpp"
        "server/*.hpp"
        "common.hpp"
        "common/utils.hpp"
)

file( GLOB RENDERER_FILES
        "renderer/*.cpp"
        "renderer/*.hpp"
        "renderer/deferred/*.cpp"
        "renderer/deferred/*.hpp"
        "renderer/deferred/passes/*.cpp"
        "renderer/deferred/passes/*.hpp"
)

set( ALL_SOURCES ${SOURCES} ${HEADERS} ${RENDERER_FILES} )

add_library( engine SHARED ${ALL_SOURCES} )

target_include_directories( engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

# ----------------------------------------------
target_compile_definitions( engine PRIVATE ENGINE_EXPORTS )

add_custom_command( TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:engine>
        $<TARGET_FILE_DIR:launcher>
)

# ----------------------------------------------
target_link_libraries( engine PUBLIC
        SDL3::SDL3
        glm::glm
        Jolt::Jolt
        nlohmann_json::nlohmann_json
        imgui::imgui
        Boost::uuid

        Diligent-BuildSettings
        Diligent-TargetPlatform
)

if ( PLATFORM_WIN32 )
    target_compile_definitions( engine INTERFACE PLATFORM_WIN32=1 )

    target_link_libraries( engine PUBLIC
            Diligent-GraphicsEngineD3D11-static
            # Diligent-GraphicsEngineVk-static
            d3d11
            dxgi
            d3dcompiler
            dxguid
    )
else ()
    if ( PLATFORM_LINUX )
        target_compile_definitions( engine INTERFACE PLATFORM_LINUX=1 )

        # target_link_libraries( engine PUBLIC
        #         Diligent-GraphicsEngineVk-static
        # )
    endif ()
endif ()

target_include_directories( engine PUBLIC
        ${Stb_INCLUDE_DIR}
        ${TINYGLTF_INCLUDE_DIRS}
        ${diligentcore_SOURCE_DIR}
)
