cmake_minimum_required(VERSION 3.10)
project(silhouette_engine)

# Enable DEBUGGING MODE AW YEAH
set(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -gstabs -Wall")
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

# Enable QT
set(CMAKE_AUTOUIC_SEARCH_PATHS ./forms)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

# Calculate the log text offset (workaround for absolute paths in cmake)
string(LENGTH ${CMAKE_SOURCE_DIR}/src/ UTIL_LOG_PATH_OFFSET)
add_compile_options(-DUTIL_LOG_PATH_OFFSET=${UTIL_LOG_PATH_OFFSET})
add_compile_options(-DSE_ENABLE_QT_WIDGET_SUPPORT)

# Add the utility library
add_library(seutil SHARED
    src/util/config.cpp
    src/util/configvalue.cpp
    src/util/debugstrings.cpp
    src/util/dirs.cpp
    src/util/hash.cpp
    src/util/log.cpp
)

# Add the silhouette library
# QT Headers have to be added to the sources list because... uh... reasons.
add_library(silhouette SHARED
    src/se/engine.cpp
    src/se/entity.cpp
    src/se/entity/camera.cpp
    src/se/entity/fpCamera.cpp
    src/se/entity/staticProp.cpp
    src/se/scene.cpp
    src/se/graphics/framebuffer.cpp
    src/se/graphics/geometry.cpp
    src/se/graphics/graphicsController.cpp
    src/se/graphics/graphicsEventHandler.cpp
    src/se/graphics/graphicsResource.cpp
    src/se/graphics/screen.cpp
    src/se/graphics/shader.cpp
    src/se/graphics/shaderProgram.cpp
    src/se/graphics/simpleRenderManager.cpp
    include/se/graphics/qtSilhouetteWidget.hpp
    src/se/graphics/qtSilhouetteWidget.cpp
    src/se/graphics/texture.cpp
    src/se/input/inputController.cpp
    src/se/logic/logicController.cpp
)
# Link to required libraries
target_link_libraries(silhouette GL SDL2 seutil GLEW Qt5::OpenGL)

# Add the test implementation
add_executable(se_test
    src/test/main.cpp
    src/test/mainwindow.cpp
)
# Link to the required libraries
target_link_libraries(se_test GL GLEW silhouette seutil pthread png Qt5::Widgets)
