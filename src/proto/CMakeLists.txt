find_package(Protobuf REQUIRED)

set(PROTO_FILES ../proto/test.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(proto
    PUBLIC
        ../proto
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(proto PRIVATE protobuf::libprotobuf)