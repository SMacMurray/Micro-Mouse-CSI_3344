cmake_minimum_required(VERSION 4.1)
project(GroupProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

add_executable(GroupProject main.cpp
        RaceCarDriver.cpp
        RaceCarDriver.h
        constants.cpp
        constants.h
        Racer.cpp
        Racer.h
        timer.cpp
        timer.h
        DisJointSet.cpp
        DisJointSet.h
        SDL_Plotter.cpp
        SDL_Plotter.h)

target_include_directories(GroupProject PRIVATE
        /opt/homebrew/include
        ${SDL2_INCLUDE_DIRS})

target_link_libraries(GroupProject PRIVATE
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer)
