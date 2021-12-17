include_directories(${GMP_INCLUDE_PATH})

add_library(utils "")
target_sources(utils
    PRIVATE
    "gini/gini.h"
    "gini/gini.cpp"
    "grena/grena.h"
    "grena/grena.cpp"
    "encode/encode.h"
    "encode/encode.cpp"
    "secure/rsa.h"
    "secure/rsa.cpp"
    "secure/md5.h"
    "secure/md5.cpp"
    "secure/signature.h"
    "secure/signature.cpp"
)

target_link_libraries(utils LINK_PUBLIC
    ${GMP_LIB}
)