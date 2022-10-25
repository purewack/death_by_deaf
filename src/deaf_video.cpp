#include "deaf.hpp"
#include <cmath>

bool done3d = false;
Camera player;
Vector3 cubePos;
float rotation;

Vector3 pos_door;
Model mod_door;
Texture2D tex_door;

void onSceneVideoFrame(){
    if(!done3d){
        done3d = true;
        player = { { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.5f, 1.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
	    rotation = 0.0;
        cubePos = { 4.0f, 1.0f, 4.0f };

        pos_door = {0.0f, 0.0f, -30.f};
        mod_door = LoadModel("gfx/door.obj");
        tex_door = LoadTexture("gfx/src/door_suv.png");
        mod_door.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex_door;
        mod_door.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = tex_door;
    }    
    BeginMode3D(player);
        DrawCube({0.5f,0.f,0.f}, 1.0f,0.1f,0.1f,RED);
        DrawCube({0.f,0.5f,0.f}, 0.1f,1.0f,0.1f,GREEN);
        DrawCube({0.f,0.f,0.5f}, 0.1f,0.1f,1.0f,BLUE);
        
        DrawCube(cubePos, 1.0f,1.0f,1.0f,WHITE);
        DrawCubeWires(cubePos, 1.0f,1.0f,1.0f,WHITE);
        DrawGrid(40, 1.0f);        // Draw a grid

        DrawModel(mod_door,pos_door,1.f,WHITE);
    EndMode3D();

    if(IsKeyDown(KEY_W)) {
        player.position.z += 0.06f*std::cos(rotation);
        player.position.x += 0.06f*std::sin(rotation);
    }
    if(IsKeyDown(KEY_S)) {
        player.position.z -= 0.06f*std::cos(rotation);
        player.position.x -= 0.06f*std::sin(rotation);
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
