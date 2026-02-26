file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(engine SHARED ${sources} ${headers})

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

auto_source_group_by_folder(headers TRUE)
auto_source_group_by_folder(sources FALSE)

# ----------------------------------------------
target_compile_definitions(engine PRIVATE ENGINE_EXPORTS)

add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:engine>
    $<TARGET_FILE_DIR:launcher>
)

# ----------------------------------------------
target_link_libraries(engine PUBLIC 
	proto
	
	SDL3::SDL3
	EnTT::EnTT	
	glm::glm
	Jolt::Jolt
	nlohmann_json::nlohmann_json
	protobuf::libprotobuf
	GameNetworkingSockets::GameNetworkingSockets
	imgui::imgui
	#Tracy::TracyClient
	
	$<$<PLATFORM_ID:Windows>:
        d3d11
        dxgi
        d3dcompiler
        dxguid
    >
)

target_include_directories(engine PUBLIC ${Stb_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

#set(SLANG_ROOT "E:/DEV/Programs/slang")
#include_directories(${SLANG_ROOT}/include)
#target_link_libraries(engine PUBLIC "${SLANG_ROOT}/lib/slang.lib")