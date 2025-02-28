cmake_minimum_required(VERSION 3.10)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(iw1x)

# Compiler Flags
set(CMAKE_C_FLAGS "-m32 -I. -fPIC -Wall -fvisibility=hidden" CACHE STRING "C flags" FORCE)
set(CMAKE_CXX_FLAGS "-m32 -I. -fPIC -Wall -fvisibility=hidden" CACHE STRING "C++ flags" FORCE)

# Linker Flags
set(CMAKE_SHARED_LINKER_FLAGS "-m32 -L/lib32" CACHE STRING "Shared linker flags" FORCE)
link_directories(/usr/lib32 /usr/lib/i386-linux-gnu)

# Compile Options
option(DEBUG_BUILD "Enable debugging information, disable optimization" OFF)
option(ENABLE_UNSAFE "Enable unsafe features (fread, fwrite...)" OFF)
option(COMPILE_SQLITE "SQLite support" OFF)
option(COMPILE_CURL "cURL support" OFF)
option(COMPILE_SSL "SSL support (hash)" OFF)

message(STATUS "Debug build: ${DEBUG_BUILD}")
message(STATUS "Unsafe features: ${ENABLE_UNSAFE}")
message(STATUS "SQLite support: ${COMPILE_SQLITE}")
message(STATUS "cURL support: ${COMPILE_CURL}")
message(STATUS "SSL support: ${COMPILE_SSL}")

if(DEBUG_BUILD)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -ggdb -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb -O0")
endif()

if(ENABLE_UNSAFE)
    add_definitions(-DENABLE_UNSAFE=1)
else()
    add_definitions(-DENABLE_UNSAFE=0)
endif()

if(COMPILE_SQLITE)
    find_library(SQLITE_LIB sqlite3 HINTS /usr/lib32 /usr/lib/i386-linux-gnu /usr/lib)
    if(SQLITE_LIB)
        add_definitions(-DCOMPILE_SQLITE=1)
        set(SQLITE_FOUND TRUE)
    else()
        message(FATAL_ERROR "SQLite requested but library not found.")
    endif()
else()
    add_definitions(-DCOMPILE_SQLITE=0)
endif()

if(COMPILE_CURL)
    find_library(CURL_LIB curl HINTS /usr/lib/i386-linux-gnu)
    if(CURL_LIB)
        add_definitions(-DCOMPILE_CURL=1)
        set(CURL_FOUND TRUE)
    else()
        message(FATAL_ERROR "cURL requested but library not found.")
    endif()
else()
    add_definitions(-DCOMPILE_CURL=0)
endif()

if(COMPILE_SSL)
    add_definitions(-DCOMPILE_SSL=1)
    set(OPENSSL_ROOT_DIR "/usr/lib/i386-linux-gnu")
    set(OPENSSL_INCLUDE_DIR "/usr/include/openssl")
    find_package(OpenSSL REQUIRED)
else()
    add_definitions(-DCOMPILE_SSL=0)
endif()

# Source files list
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/hook.cpp
    ${CMAKE_SOURCE_DIR}/src/iw1x.cpp
    ${CMAKE_SOURCE_DIR}/deps/qvsnprintf.c
    ${CMAKE_SOURCE_DIR}/src/gsc.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_entity.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_player.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_bots.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_weapons.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_exec.cpp
    ${CMAKE_SOURCE_DIR}/src/gsc_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/jump.cpp
)

if(SQLITE_FOUND)
    list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/src/gsc_sqlite.cpp)
    # append gsc_sqlite.cpp to SOURCES list if library found
endif()

if(CURL_FOUND)
    list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/src/gsc_curl.cpp)
endif()

# Include directories for searching
include_directories(${CMAKE_SOURCE_DIR}/src)

# add shared library to project
add_library(iw1x SHARED ${SOURCES})

# Output properties
set_target_properties(iw1x PROPERTIES
    OUTPUT_NAME "iw1x"
    PREFIX ""
    SUFFIX ".so"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Link libraries
target_link_libraries(iw1x
    dl
    pthread
)

if(SQLITE_FOUND)
    target_link_libraries(iw1x ${SQLITE_LIB})
endif()

if(CURL_FOUND)
    target_link_libraries(iw1x ${CURL_LIB})
endif()

if(COMPILE_SSL)
    target_link_libraries(iw1x OpenSSL::SSL OpenSSL::Crypto)
endif()
