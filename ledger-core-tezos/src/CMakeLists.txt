cmake_minimum_required(VERSION 3.12)

# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(ANDROID_CPP_FEATURES exceptions)

# Automatically add API files to the library
file(GLOB API_HEADER_FILES "${PROJECT_SOURCE_DIR}/inc/tezos/api/*.hpp")

# Configure JNI target
if (TARGET_JNI)
    file(GLOB JNI_SRC_FILES "${PROJECT_SOURCE_DIR}/inc/tezos/jni/*.cpp")
    file(GLOB JNI_HEADER_FILES "${PROJECT_SOURCE_DIR}/inc/tezos/jni/*.hpp")

    list(APPEND API_HEADER_FILES ${JNI_HEADER_FILES})
endif ()

# Add files to compile to the project
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tezos/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/inc/tezos/*.hpp)

add_library(ledger-core-tezos-obj OBJECT ${SRC_FILES} ${HEADER_FILES} ${API_HEADER_FILES})

# enable static analyzers if requested
include(StaticAnalyzers)
toggle_static_analyzers(ledger-core-tezos-obj)

# set compiler warnings
include(CompilerWarnings)
toggle_compiler_warnings(ledger-core-tezos-obj)

# link the ledger-core library
target_link_libraries(ledger-core-tezos-obj PRIVATE ledger-core-obj)

target_include_directories(ledger-core-tezos-obj PUBLIC
  ${PROJECT_SOURCE_DIR}/inc
  ${PROJECT_SOURCE_DIR}/inc/tezos/api
)
