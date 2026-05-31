cmake_minimum_required(VERSION 3.14)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 17)

# Automatically fetch Raylib so you don't have to install it manually!
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(raylib)

add_executable(SnakeGame SnakeGame.cpp)
target_link_libraries(SnakeGame PRIVATE raylib)

# Suppress console window on release builds for Windows
if (WIN32)
    set_target_properties(SnakeGame PROPERTIES WIN32_EXECUTABLE ON)
    if (MSVC)
        # Tell MSVC to use the standard main function instead of WinMain
        set_property(TARGET SnakeGame APPEND_STRING PROPERTY LINK_FLAGS " /ENTRY:mainCRTStartup")
    endif()
endif()
