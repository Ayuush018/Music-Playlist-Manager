cmake_minimum_required(VERSION 3.12)
project(MusicPlaylistManager VERSION 1.0.0 LANGUAGES CXX)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/Song.cpp
    src/DoublyLinkedList.cpp
    src/PlaylistManager.cpp
)

# Compiler warnings for high code quality
if(MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Executable target
add_executable(MusicPlaylistManager ${SOURCES})

# Target include directories
target_include_directories(MusicPlaylistManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
