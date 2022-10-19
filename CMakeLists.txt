cmake_minimum_required(VERSION 3.10.0)

project(deaf)

add_subdirectory(deps/libpd)

# add_executable($(PROJECT_NAME) src/main.cpp src/audio.cpp src/calls.cpp)

add_executable(${PROJECT_NAME} src/test.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC deps/libpd/pure-data/src PUBLIC deps/libpd/libpd_wrapper)

target_link_directories(${PROJECT_NAME} PUBLIC deps/libpd)

target_link_libraries(${PROJECT_NAME} libpd)