#include <stdio.h>
#include <iostream>
#include "z_libpd.h"
#include "RtAudio.h"
#include "sol.hpp"
#include "raylib.h"

void pdprint(const char *s) {
    printf("%s", s);
}
int main(){
    std::cout << "test CMAKE" << std::endl;

    std::cout << "libpd" << std::endl;
    libpd_set_printhook(pdprint);
    libpd_init();

    std::cout << "rtaudio" << std::endl;
    std::cout << "\nRtAudio Version " << RtAudio::getVersion() << std::endl;

    std::cout << "sol2" << std::endl;
    sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.script("print(\"hello sol2\")");

    InitWindow(800, 450, "raylib [core] example - basic window");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("raylib", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}