set(PROTO_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/test.proto
)

add_library(proto STATIC ${PROTO_FILES})

set_target_properties(proto PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(proto PUBLIC protobuf::libprotobuf)

target_include_directories(proto PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

protobuf_generate(
    TARGET proto
    PROTOS ${PROTO_FILES}
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
)