cmake_minimum_required(VERSION 3.10.0)

project(deaf)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps/libpd)
add_subdirectory(deps/rtaudio)

add_executable(${PROJECT_NAME} src/test.cpp)


target_include_directories(${PROJECT_NAME} 
    PUBLIC deps/libpd/pure-data/src 
    PUBLIC deps/libpd/libpd_wrapper
    PUBLIC deps/rtaudio
)

target_link_directories(${PROJECT_NAME} 
    PUBLIC deps/libpd
    PUBLIC deps/rtaudio
)

target_link_libraries(${PROJECT_NAME} libpd rtaudio)