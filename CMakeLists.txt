cmake_minimum_required(VERSION 3.10.0)

project(deaf)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_CXX_STANDARD 17)

find_package(Lua51 REQUIRED)
include_directories(/usr/local/include ${LUA_INCLUDE_DIR})

add_subdirectory(deps/libpd)
add_subdirectory(deps/rtaudio)
add_subdirectory(deps/sol2)

add_executable(${PROJECT_NAME} src/test.cpp)


target_include_directories(${PROJECT_NAME} 
    PUBLIC deps/libpd/pure-data/src 
    PUBLIC deps/libpd/libpd_wrapper
    PUBLIC deps/rtaudio
    PUBLIC deps/sol2/include/sol
)

target_link_directories(${PROJECT_NAME} 
    PUBLIC deps/libpd
    PUBLIC deps/rtaudio
    PUBLIC deps/sol2
)

target_link_libraries(${PROJECT_NAME} libpd rtaudio sol2 ${LUA_LIBRARIES})