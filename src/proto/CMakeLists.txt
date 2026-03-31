set(PROTO_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/test.proto
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

set_target_properties(proto PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(proto PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(proto PUBLIC protobuf::libprotobuf)