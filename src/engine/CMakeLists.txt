file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(engine SHARED ${sources} ${headers})

function(auto_source_group_by_folder FILES IS_HEADER)
    foreach(file ${${FILES}})
        get_filename_component(dir "${file}" PATH)
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${dir}")

        # Split relative path and capitalize parts
        string(REPLACE "/" ";" parts "${rel_path}")
        set(group_parts "")
        foreach(part ${parts})
            string(SUBSTRING "${part}" 0 1 first)
            string(SUBSTRING "${part}" 1 -1 rest)
            string(TOUPPER "${first}" first)
            set(capitalized "${first}${rest}")
            list(APPEND group_parts "${capitalized}")
        endforeach()

        # For headers, append "Headers" to group
        if(IS_HEADER)
            list(APPEND group_parts "Headers")
        endif()

        # Rejoin with backslashes
        string(REPLACE ";" "\\" group "${group_parts}")

        source_group("${group}" FILES "${file}")
    endforeach()
endfunction()

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

auto_source_group_by_folder(headers TRUE)
auto_source_group_by_folder(sources FALSE)

# ------------------
target_compile_definitions(engine PRIVATE ENGINE_EXPORTS)

add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:engine>
    $<TARGET_FILE_DIR:launcher>
)

# ------------------
target_link_libraries(engine PUBLIC 
	proto
	glad
	SDL3::SDL3
	EnTT::EnTT	
	glm::glm
	Jolt::Jolt
	nlohmann_json::nlohmann_json
	protobuf::libprotobuf
	GameNetworkingSockets::GameNetworkingSockets
	imgui::imgui
)

target_include_directories(engine PUBLIC ${Stb_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})