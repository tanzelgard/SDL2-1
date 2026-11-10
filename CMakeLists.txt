cmake_minimum_required(VERSION 3.15)
project(MarioSDL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

add_executable(MarioSDL WIN32
    src/main.cpp src/Player.cpp src/Player.h
    src/Physics.h src/World.h src/World.cpp)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:MarioSDL>/assets)

target_link_libraries(MarioSDL PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image)

if (MSVC)
    target_compile_options(MarioSDL PRIVATE /W4 /wd4244)
else()

    target_compile_options(MarioSDL PRIVATE -Wall -Wextra -Wpedantic)
endif()