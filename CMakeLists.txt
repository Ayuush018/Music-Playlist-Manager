cmake_minimum_required(VERSION 3.16)
project(MusicPlaylistManager VERSION 1.1.0 LANGUAGES CXX)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Common backend sources
set(BACKEND_SOURCES
    src/Song.cpp
    src/DoublyLinkedList.cpp
    src/PlaylistManager.cpp
)

# Compiler warning flags
if(MSVC)
    add_compile_options(/W4 /EHsc)
else()
    add_compile_options(-Wall -Wextra)
endif()

# ===================================================================
# Target 1: MusicPlaylistManagerCLI (Original Console Application)
# ===================================================================
add_executable(MusicPlaylistManagerCLI
    src/main_cli.cpp
    ${BACKEND_SOURCES}
)
target_include_directories(MusicPlaylistManagerCLI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Maintain backward-compatible legacy target name
add_executable(MusicPlaylistManager
    src/main.cpp
    ${BACKEND_SOURCES}
)
target_include_directories(MusicPlaylistManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ===================================================================
# Target 2: MusicPlaylistManagerGUI (Qt 6 Desktop Application)
# ===================================================================
option(BUILD_GUI "Build the Qt 6 Modern Desktop GUI application" ON)

if(BUILD_GUI)
    find_package(Qt6 COMPONENTS Widgets QUIET)

    if(Qt6_FOUND)
        message(STATUS "Qt 6 Widgets found! Configuring MusicPlaylistManagerGUI target...")
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTORCC ON)
        set(CMAKE_AUTOUIC ON)

        set(GUI_SOURCES
            src/main_gui.cpp
            src/MainWindow.cpp
            include/MainWindow.h
            ${BACKEND_SOURCES}
        )

        add_executable(MusicPlaylistManagerGUI
            ${GUI_SOURCES}
        )

        target_include_directories(MusicPlaylistManagerGUI PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

        target_link_libraries(MusicPlaylistManagerGUI PRIVATE
            Qt6::Widgets
        )

        # ===================================================================
        # Target 3: Automated Integration Tests
        # ===================================================================
        option(BUILD_TESTS "Build automated GUI and backend test suite" ON)
        if(BUILD_TESTS)
            add_executable(verify_gui_and_backend
                tests/verify_gui_and_backend.cpp
                src/MainWindow.cpp
                include/MainWindow.h
                ${BACKEND_SOURCES}
            )
            target_include_directories(verify_gui_and_backend PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/include
            )
            target_link_libraries(verify_gui_and_backend PRIVATE
                Qt6::Widgets
            )
        endif()
    else()
        message(WARNING "Qt 6 was not found in environment paths. "
                        "MusicPlaylistManagerCLI will be built successfully. "
                        "To build MusicPlaylistManagerGUI, please install Qt 6 and specify: "
                        "cmake -S . -B build -DCMAKE_PREFIX_PATH=<path_to_qt6>")
    endif()
endif()
