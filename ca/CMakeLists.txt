include_directories(${PROJECT_BINARY_DIR}/include)

include_directories(${BRPC_INCLUDE_PATH})

include_directories(${GFLAGS_INCLUDE_PATH})

include_directories(${LEVELDB_INCLUDE_PATH})

include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/utils)

# 打包配置文件
file(GLOB ALL_COPY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/*")
file(COPY ${ALL_COPY_FILES} DESTINATION ${PROJECT_BINARY_DIR}/ca/config)

add_executable (ca "")
target_sources(ca
    PRIVATE
    ${PROTO_HDRS}
    ${PROTO_SRCS}
    "main.cpp"
    "identity_service/identity_service.h"
    "identity_service/identity_service.cpp"
)

target_link_libraries(ca LINK_PUBLIC 
    ${BRPC_LIB} 
    ${DYNAMIC_LIB}
    utils
)