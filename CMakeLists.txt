cmake_minimum_required(VERSION 3.10)

project("RayTracer")

set (BUILD_SHARED_LIBS FALSE)

# SFML
add_subdirectory(./include/SFML)
include_directories(./include/SFML/include)
set(SFML_LIBRARY sfml-main sfml-graphics sfml-audio)

# Assimp
add_subdirectory(./include/assimp)
include_directories(./include/assimp/include)
set(ASSIMP_LIBRARY assimp::assimp)

set(SOURCE_FILES    src/Camera/Camera.cc
                    src/Dispatcher/Dispatcher.cc
                    src/Engine/Color.cc
                    src/Engine/Engine.cc
                    src/Light/PointLight.cc
                    src/Loader/AssimpLoader.cc
                    src/Geometry/Geometry.cc
                    src/main.cc)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARY} ${ASSIMP_LIBRARY})