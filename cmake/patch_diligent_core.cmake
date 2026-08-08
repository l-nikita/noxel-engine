if ( NOT DEFINED DILIGENT_CORE_SOURCE_DIR )
    message( FATAL_ERROR "DILIGENT_CORE_SOURCE_DIR is required" )
endif ()

set( third_party_cmake "${DILIGENT_CORE_SOURCE_DIR}/ThirdParty/CMakeLists.txt" )
file( READ "${third_party_cmake}" content )

set( original [=[add_subdirectory(abseil-cpp EXCLUDE_FROM_ALL)
set_directory_root_folder("abseil-cpp" "DiligentCore/ThirdParty")]=] )
set( replacement [=[if (WEBGPU_SUPPORTED)
    add_subdirectory(abseil-cpp EXCLUDE_FROM_ALL)
    set_directory_root_folder("abseil-cpp" "DiligentCore/ThirdParty")
endif()]=] )

string( REPLACE "${original}" "${replacement}" content "${content}" )
file( WRITE "${third_party_cmake}" "${content}" )
