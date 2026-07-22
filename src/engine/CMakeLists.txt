file( GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "assetsystem/*.cpp"
        "client/*.cpp"
        "common/*.cpp"
        "dbg/*.cpp"
        "engine/*.cpp"
        "filesystem/*.cpp"
        "gui/*.cpp"
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
        "gui/*.hpp"
        "input/*.hpp"
        "networking/*.hpp"
        "physics/*.hpp"
        "server/*.hpp"
        "common.hpp"
        "utils.hpp"
)

file( GLOB RENDERER_FILES
        "renderer/*.cpp"
        "renderer/*.hpp"
)

file( GLOB DX11_FILES
        "renderer/directx11/*.cpp"
        "renderer/directx11/*.hpp"
)

file( GLOB VULKAN_FILES
        "renderer/vulkan/*.cpp"
        "renderer/vulkan/*.hpp"
)

set( ALL_SOURCES ${SOURCES} ${HEADERS} ${RENDERER_FILES} ${VULKAN_FILES} )

if ( WIN32 )
    list( APPEND ALL_SOURCES ${DX11_FILES} )
endif ()

add_library( engine SHARED ${ALL_SOURCES} )

target_include_directories( engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

if ( WIN32 )
    target_compile_definitions( engine PRIVATE DX11 )
endif ()

# ----------------------------------------------
target_compile_definitions( engine PRIVATE ENGINE_EXPORTS )

add_custom_command( TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:engine>
        $<TARGET_FILE_DIR:launcher>
)

# ----------------------------------------------
target_link_libraries( engine PUBLIC
        proto
        SDL3::SDL3
        EnTT::EnTT
        glm::glm
        Jolt::Jolt
        nlohmann_json::nlohmann_json
        protobuf::libprotobuf
        GameNetworkingSockets::static
        imgui::imgui

        $<$<PLATFORM_ID:Windows>:
        d3d11
        dxgi
        d3dcompiler
        dxguid
        >

        # Vulkan::Vulkan
)

target_include_directories( engine PUBLIC ${Stb_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} )