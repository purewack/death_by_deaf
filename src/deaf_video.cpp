#include "deaf.hpp"

bool done3d = false;
Camera player;
Vector3 cubePos;

void onSceneVideoFrame(){
    if(!done3d){
        done3d = true;
        player = { { 0.0f, 1.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
        cubePos = { 0.0f, 0.5f, 0.0f };
    }    
    BeginMode3D(player);
        DrawCube(cubePos, 1.0f,1.0f,1.0f,WHITE);
        DrawCubeWires(cubePos, 1.0f,1.0f,1.0f,WHITE);
        DrawGrid(40, 1.0f);        // Draw a grid
    EndMode3D();

    if(IsKeyDown(KEY_W)) {
        player.target.z -= 0.2f;
        player.position.z -= 0.2f;
    }
    if(IsKeyDown(KEY_S)) {
        player.target.z += 0.2f;
        player.position.z += 0.2f;
    }
}