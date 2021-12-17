# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.

cmake_minimum_required (VERSION 2.8.11)
project (blockchain)

# Recurse into the "Hello" and "Demo" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.

execute_process(
    COMMAND bash -c "find ${PROJECT_SOURCE_DIR} -type d -regex \".*output/include$\" | head -n1 | xargs dirname | tr -d '\n'"
    OUTPUT_VARIABLE OUTPUT_PATH
)

message("OUTPUT PATH:" ${OUTPUT_PATH})

set(CMAKE_PREFIX_PATH ${OUTPUT_PATH})

find_package(Protobuf 3 REQUIRED)

#设置输出路径
SET(INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
if(EXISTS "${CMAKE_BINARY_DIR}/include" AND IS_DIRECTORY "${CMAKE_BINARY_DIR}/include")
        SET(PROTO_META_BASE_DIR ${INCLUDE_DIR})
else()
        file(MAKE_DIRECTORY ${INCLUDE_DIR})
        SET(PROTO_META_BASE_DIR ${INCLUDE_DIR})
endif()

#设置protoc的搜索路径
LIST(APPEND PROTO_FLAGS -I${CMAKE_SOURCE_DIR}/proto)
#获取需要编译的proto文件
file(GLOB_RECURSE SRC_PROTOS ${CMAKE_SOURCE_DIR}/proto/*.proto)
set(PROTO_SRCS "")
set(PROTO_HDRS "")

foreach(proto_file ${SRC_PROTOS})
        get_filename_component(FIL_WE ${proto_file} NAME_WE)

        list(APPEND PROTO_SRCS "${PROJECT_BINARY_DIR}/include/${FIL_WE}.pb.cc")
        list(APPEND PROTO_HDRS "${PROJECT_BINARY_DIR}/include/${FIL_WE}.pb.h")

		# 使用自定义命令
        add_custom_command(
          OUTPUT "${PROJECT_BINARY_DIR}/include/${FIL_WE}.pb.cc"
                 "${PROJECT_BINARY_DIR}/include/${FIL_WE}.pb.h"
          COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
          ARGS --cpp_out  ${PROTO_META_BASE_DIR}
            -I ${CMAKE_SOURCE_DIR}/proto
            ${proto_file}
          DEPENDS ${proto_file}
          COMMENT "Running C++ protocol buffer compiler on ${proto_file}"
          VERBATIM
        )
endforeach()

# 设置文件属性为 GENERATED
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

# 添加自定义target
add_custom_target(generate_pb2cpp ALL
                DEPENDS ${PROTO_SRCS} ${PROTO_HDRS}
                COMMENT "generate pb2cpp target"
                VERBATIM
)


find_library(THRIFT_LIB NAMES thrift)
if (NOT THRIFT_LIB)
    set(THRIFT_LIB "")
endif()
find_library(THRIFTNB_LIB NAMES thriftnb)
if (NOT THRIFTNB_LIB)
    set(THRIFTNB_LIB "")
endif()

find_path(BRPC_INCLUDE_PATH NAMES brpc/server.h)
if(LINK_SO)
    find_library(BRPC_LIB NAMES brpc)
else()
    find_library(BRPC_LIB NAMES libbrpc.a brpc)
endif()
if((NOT BRPC_INCLUDE_PATH) OR (NOT BRPC_LIB))
    message(FATAL_ERROR "Fail to find brpc")
endif()

find_path(GMP_INCLUDE_PATH NAMES gmp.h gmpxx.h)
find_library(GMP_LIB NAMES libgmp.a libgmpxx.a gmp gmpxx)

# include_directories(${BRPC_INCLUDE_PATH})
find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIBRARY))
    message(FATAL_ERROR "Fail to find gflags")
endif()
# include_directories(${GFLAGS_INCLUDE_PATH})
execute_process(
    COMMAND bash -c "grep \"namespace [_A-Za-z0-9]\\+ {\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $2}' | tr -d '\n'"
    OUTPUT_VARIABLE GFLAGS_NS
)
if(${GFLAGS_NS} STREQUAL "GFLAGS_NAMESPACE")
    execute_process(
        COMMAND bash -c "grep \"#define GFLAGS_NAMESPACE [_A-Za-z0-9]\\+\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $3}' | tr -d '\n'"
        OUTPUT_VARIABLE GFLAGS_NS
    )
endif()

set(CMAKE_CPP_FLAGS "${DEFINE_CLOCK_GETTIME} -DGFLAGS_NS=${GFLAGS_NS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

if(CMAKE_VERSION VERSION_LESS "3.1.3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif()
# include_directories(${LEVELDB_INCLUDE_PATH})

find_package(OpenSSL)
# include_directories(${OPENSSL_INCLUDE_DIR})


set(DYNAMIC_LIB
    ${CMAKE_THREAD_LIBS_INIT}
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${LEVELDB_LIB}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    ${THRIFT_LIB}
    ${THRIFTNB_LIB}
    dl
)
add_subdirectory(ca)
add_subdirectory (store_node)
# add_subdirectory(leveldb)
add_subdirectory(consen_node)
add_subdirectory(utils)

# add_subdirectory(incubator-brpc)