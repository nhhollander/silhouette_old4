cmake_minimum_required(VERSION 3.10)
project(silhouette_engine)

# Enable DEBUGGING MODE AW YEAH
set(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -gstabs")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -gstabs")

# Set compilation flags
SET(CMAKE_CXX_FLAGS "-Wall")

# Prevent in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
# Set the linker search directory
link_directories(../lib)
# Set the include directory
include_directories(include)
# Use c++ 17
set(CMAKE_CXX_STANDARD 17)

# Calculate the log text offset (workaround for absolute paths in cmake)
string(LENGTH ${CMAKE_SOURCE_DIR}/src/ UTIL_LOG_PATH_OFFSET)
add_compile_options(-DUTIL_LOG_PATH_OFFSET=${UTIL_LOG_PATH_OFFSET})

# Add the utility library
add_library(seutil SHARED
    src/util/config.cpp
    src/util/configvalue.cpp
    src/util/debugstrings.cpp
    src/util/hash.cpp
    src/util/log.cpp
)

# Add the silhouette library
add_library(silhouette SHARED
    src/se/engine.cpp
    src/se/graphics/graphicsController.cpp
)
# Enable the internal library flag
#target_compile_options(silhouette PUBLIC -DSE_INTERNAL_BUILD)
# Link to required libraries
target_link_libraries(silhouette SDL2 seutil GLEW)

# Add the test implementation
add_executable(se_test
    src/test/main.cpp
)
# Link to the required libraries
target_link_libraries(se_test GL GLEW silhouette seutil pthread)
