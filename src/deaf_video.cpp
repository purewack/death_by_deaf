#include "deaf.hpp"
#include <cmath>

bool done3d = false;
Camera player;
Vector3 cubePos;
float rotation;

void onSceneVideoFrame(){
    if(!done3d){
        done3d = true;
        player = { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
	      rotation = 0.0;
        cubePos = { 4.0f, 1.0f, 4.0f };
    }    
    BeginMode3D(player);

        DrawCube({0.5f,0.f,0.f}, 1.0f,0.1f,0.1f,RED);
        DrawCube({0.f,0.5f,0.f}, 0.1f,1.0f,0.1f,GREEN);
        DrawCube({0.f,0.f,0.5f}, 0.1f,0.1f,1.0f,BLUE);
        
        DrawCube(cubePos, 1.0f,1.0f,1.0f,WHITE);
        DrawCubeWires(cubePos, 1.0f,1.0f,1.0f,WHITE);
        DrawGrid(40, 1.0f);        // Draw a grid
    EndMode3D();

    if(IsKeyDown(KEY_W)) {
        player.position.z += 0.2f*std::cos(rotation);
        player.position.x += 0.2f*std::sin(rotation);
    }
    if(IsKeyDown(KEY_S)) {
        player.position.z -= 0.2f*std::cos(rotation);
        player.position.x -= 0.2f*std::sin(rotation);
    }
    if(IsKeyDown(KEY_A)) {
        rotation += (2.0f/360.0f)*2.0f*3.1415f;
    }
    if(IsKeyDown(KEY_D)) {
        rotation -= (2.0f/360.0f)*2.0f*3.1415f;
    }

  	player.target.x = player.position.x + std::sin(rotation);
  	player.target.z = player.position.z + std::cos(rotation);
}
