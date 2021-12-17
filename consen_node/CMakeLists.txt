# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
# add_library (Hello hello.cxx)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
# target_include_directories (Hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.

include_directories(${PROJECT_BINARY_DIR}/include)

include_directories(${BRPC_INCLUDE_PATH})

include_directories(${GFLAGS_INCLUDE_PATH})

include_directories(${LEVELDB_INCLUDE_PATH})

include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/utils)


# 打包配置文件
file(GLOB ALL_COPY_FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/*")
file(COPY ${ALL_COPY_FILES} DESTINATION ${PROJECT_BINARY_DIR}/consen_node/config)

add_executable (consensus "")
target_sources(consensus
    PRIVATE
    ${PROTO_HDRS}
    ${PROTO_SRCS}
    "main.cpp"
    "global/global.h"
    "global/global.cpp"
    "init/consen_node_identity.h"
    "init/consen_node_identity.cpp"
    "raft/raft.h"
    "raft/raft.cpp"
)

# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
# target_link_libraries (helloDemo LINK_PUBLIC Hello)
target_link_libraries(consensus LINK_PUBLIC 
    ${BRPC_LIB} 
    ${DYNAMIC_LIB}
    utils    
)