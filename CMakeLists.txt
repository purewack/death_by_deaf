cmake_minimum_required(VERSION 3.10.0)

project(deaf)

add_executable($(PROJECT_NAME) src/main.cpp src/audio.cpp src/calls.cpp)