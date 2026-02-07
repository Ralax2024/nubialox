cmake_minimum_required(VERSION 3.16)
project(nubialox_cpp VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nubialox_game src/main.cpp)

if(UNIX)
  add_custom_command(
    TARGET nubialox_game
    POST_BUILD
    COMMAND /usr/bin/env bash -c "truncate -s 200M '$<TARGET_FILE:nubialox_game>'"
    COMMENT "Padding executable to ~200 MB as requested"
  )
endif()
