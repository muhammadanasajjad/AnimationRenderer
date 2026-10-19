cmake_minimum_required(VERSION 3.20)

project(AnimationRasteriser LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(AnimationRasteriser
    ${SOURCES}
    external/glad/src/glad.c
)

target_include_directories(AnimationRasteriser PRIVATE
    external/glad/include
    include
)

target_link_libraries(AnimationRasteriser PRIVATE
    glfw
)
