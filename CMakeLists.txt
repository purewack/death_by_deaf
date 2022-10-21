cmake_minimum_required(VERSION 3.10.0)

project(deaf)
#output to bin folder with all scripts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wno-narrowing )

#for sol2
find_package(Lua51 REQUIRED)
include_directories(/usr/local/include ${LUA_INCLUDE_DIR})

add_subdirectory(deps/libpd)
add_subdirectory(deps/rtaudio)
add_subdirectory(deps/sol2)
add_subdirectory(deps/raylib)

add_executable(${PROJECT_NAME} src/main.cpp src/calls.cpp src/audio.cpp)

target_include_directories(${PROJECT_NAME} 
	PUBLIC include
    PUBLIC deps/libpd/pure-data/src 
    PUBLIC deps/libpd/libpd_wrapper
    PUBLIC deps/rtaudio
    PUBLIC deps/sol2/include/sol
    PUBLIC deps/raylib/src
)

target_link_directories(${PROJECT_NAME} 
    PUBLIC deps/libpd
    PUBLIC deps/rtaudio
    PUBLIC deps/sol2
    PUBLIC deps/raylib
)

target_link_libraries(${PROJECT_NAME} 
    libpd 
    rtaudio 
    sol2 
    ${LUA_LIBRARIES}
    raylib    
)


# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
