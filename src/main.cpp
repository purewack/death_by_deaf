#include "deaf.hpp"
#include "raymath.h"
#include "rlgl.h"

bool colDetPlayerEdge(Vector2 vs, Vector2 ve);
bool colDetPlayerTri(Vector2 v1,Vector2 v2,Vector2 v3);
bool colDetPlayer(Mesh &cc);
Vector2 player = {0,0};
float playerR = 1.0;

int ss = 6;
int dd = 0;

Vector2 normalLine(Vector2 v1, Vector2 v2){
	return {-(v2.y-v1.y),(v2.x-v1.x)};
};

Vector2 conditionPoint(Vector2 v){
	v.y *= -1;
	auto mm = MatrixTranslate(ss/2,ss/2,0);
	auto p1 = Vector2Transform(v,mm);
	p1 = Vector2Scale(p1,dd);
	return p1;
}

int main(int argc, char* argv[]) 
{
	InitWindow(S_WIDTH,S_HEIGHT_T,"DEAF Engine");
	SetTargetFPS(
		#ifdef WIN32
		120
		#else
		60
		#endif
	);

	Vector2 w1 = {-0.9,2.1};
	Vector2 w2 = {-0.5,0.2};
	Vector2 w3 = {1,0.5};
	dd = std::min(S_WIDTH,S_HEIGHT)/ss;
	
	while(not WindowShouldClose()){
        
        BeginDrawing();
        ClearBackground(BLACK);
			
			for(int d=0; d<ss; d++){
				DrawLine(d*dd,0,d*dd,S_HEIGHT,d != ss/2 ? GRAY : RED);
				DrawLine(0,d*dd,S_WIDTH,d*dd,d != ss/2 ? GRAY : RED);
			}

			auto col = colDetPlayerTri(w1,w2,w3);
			auto p1 = conditionPoint(w1);
			auto p2 = conditionPoint(w2);
			auto p3 = conditionPoint(w3);
			DrawTriangleLines(p1,p2,p3,col ? RED : YELLOW);

			auto pp = conditionPoint(player);
			DrawCircleLines(pp.x,pp.y,playerR*dd,BLUE);
			DrawPixelV(pp,BLUE);

			if(IsKeyDown(KEY_UP)) player.y += 0.01;
			if(IsKeyDown(KEY_DOWN)) player.y -= 0.01;
			if(IsKeyDown(KEY_LEFT)) player.x -= 0.01;
			if(IsKeyDown(KEY_RIGHT)) player.x += 0.01;

	 	EndDrawing();   
	}

	return 0;
}


bool colDetPlayerEdge(Vector2 vs, Vector2 ve){
	#define LOG(X) 

	Vector2 ppos = player; //player pos
	float prr = playerR; //player radius

	//find closest point to circle
	float c1 = Vector2Distance(ppos,vs);
	float c2 = Vector2Distance(ppos,ve);
	Vector2 cp = (c1 < c2) ? vs : ve; //closest point

	auto ax_close = Vector2Subtract(ppos,cp);

	//normal to line calculation
	auto ax_norm = Vector2Subtract({ve.y,ve.x},{vs.y,vs.x});
	ax_norm.x *= -1;
	ax_norm = Vector2Normalize(ax_norm);
	
	// DrawLineV(conditionPoint({ppos}),conditionPoint(cp),LIME); // close line point
	// DrawLineV(conditionPoint(ax_norm),conditionPoint({0}),PURPLE); // line normal

	// projected circle points on axis of test	
	auto close_p1 = Vector2DotProduct( ppos, ax_close )+prr;
	auto close_p2 = Vector2DotProduct( ppos, ax_close )-prr;
	auto norm_p1 = Vector2DotProduct( ppos, ax_norm )+prr;
	auto norm_p2 = Vector2DotProduct( ppos, ax_norm )-prr;
	
	// projected wall points on axis of test
	auto close_w1 = Vector2DotProduct( vs, ax_close );
	auto close_w2 = Vector2DotProduct( ve, ax_close );
	auto norm_w = Vector2DotProduct( vs, ax_norm );

	bool shadow_close = false;
	if(std::max(close_w1,close_w2) >= std::min(close_p1,close_p2)) 
		shadow_close = true;
	
	bool shadow_norm = false;
	if(norm_w >= std::min(norm_p1,norm_p2) && norm_w < std::max(norm_p1,norm_p2)) 
		shadow_norm = true;

	// const char* tt = (shadow_close ? "CLOSE" : (shadow_norm ? "NORM" : "--"));
	// DrawText(tt,0,0,16,WHITE);
	// const char* col = (shadow_close && shadow_norm ? "COL" : "--");
	// DrawText(col,0,16,16,WHITE);
	// // DrawText(STR(close_p1).c_str(),0,0,16,WHITE);
	// // DrawText(STR(close_p2).c_str(),0,16,16,WHITE);
	// // DrawText(STR(close_w1).c_str(),0,16*2,16,WHITE);
	// // DrawText(STR(close_w2).c_str(),0,16*3,16,WHITE);
	// // DrawText(STR(norm_p1).c_str(),0,16*4,16,WHITE);
	// // DrawText(STR(norm_p2).c_str(),0,16*5,16,WHITE);
	// // DrawText(STR(norm_w1).c_str(),0,16*6,16,WHITE);
	// // DrawText(STR(norm_w2).c_str(),0,16*7,16,WHITE);
	// DrawLineV(conditionPoint({close_p1,-1.1}),conditionPoint({close_p2,-1.12}),LIME);
	// DrawLineV(conditionPoint({close_w1,-1.2}),conditionPoint({close_w2,-1.22}),ORANGE);
	// DrawLineV(conditionPoint({norm_p1,-1.8}),conditionPoint({norm_p2,-1.82}),PURPLE);
	// DrawLineV(conditionPoint({norm_w,-1.91}),conditionPoint({norm_w,-1.92}),YELLOW);
	// // // if( pmax > w1 && pmin <= w) {
	// //     // return true;
	// // // }
	// // 

	return shadow_close && shadow_norm;
}

bool colDetPlayerTri(Vector2 v1,Vector2 v2,Vector2 v3){
	return colDetPlayerEdge(v1,v2) || colDetPlayerEdge(v2,v3) || colDetPlayerEdge(v3,v1);
}

bool colDetPlayer(Mesh &cc){
	// auto r = MatrixRotate({0,1.f,0},30.f);
	// auto mx = MatrixTranslate(0,0,3.f);
	// mx = MatrixMultiply(mx,r);
	// mx = MatrixIdentity();
	// auto ccc = cc.vertexCount; //vertex count of coliding mesh
	// auto ttt = cc.triangleCount;

	// // //acquire mesh edges projected from top
	// // std::vector<Vector2> normals;
	// // for(int i=0; i<ccc; i++){
	// // 	//edge point 1
	// // 	auto vs = Vector3Transform({
	// // 		cc.vertices[(i*3)+0],
	// // 		cc.vertices[(i*3)+1],
	// // 		cc.vertices[(i*3)+2]
	// // 	},mx);

	// // 	//edge point 2
	// // 	auto ve = Vector3Transform({
	// // 		cc.vertices[((i*6)+0)%(ccc*3)],
	// // 		cc.vertices[((i*6)+1)%(ccc*3)],
	// // 		cc.vertices[((i*6)+2)%(ccc*3)]
	// // 	},mx);

	// // 	//debug draw line
	// // 	DrawLine3D(vs,ve,YELLOW);
	// // 	DrawCube(vs,0.1,0.1,0.1,GREEN);

	// // 	// //edge normals
	// // 	// auto norm = Vector2Subtract(Vector2{ve.x,ve.z},Vector2{vs.x,vs.z});
	// // 	// norm.x *= -1;
	// // 	// normals.push_back(norm);
	// // }

	// // //debug draw shape
	// for(int i=0; i<ttt; i++){
	// 	//loop thru tris' vertexes
	// 	//for(int j=0; j<3; j++){
	// 		int j=0;
	// 		Vector3 s = {
	// 			cc.vertices[cc.indices[i*3 + j]*3 +0],
	// 			cc.vertices[cc.indices[i*3 + j]*3 +1],
	// 			cc.vertices[cc.indices[i*3 + j]*3 +2]
	// 		};
	// 		Vector3 e = {
	// 			cc.vertices[cc.indices[i*3 + (j+1)%3]*3 +0],
	// 			cc.vertices[cc.indices[i*3 + (j+1)%3]*3 +1],
	// 			cc.vertices[cc.indices[i*3 + (j+1)%3]*3 +2]
	// 		};
	// 		Vector3 t = {
	// 			cc.vertices[cc.indices[i*3 + (j+2)%3]*3 +0],
	// 			cc.vertices[cc.indices[i*3 + (j+2)%3]*3 +1],
	// 			cc.vertices[cc.indices[i*3 + (j+2)%3]*3 +2]
	// 		};
	// 		//DrawTriangle3D(s,e,t,{128,128,(192*i)%255,255});

	// 		DrawLine3D(s,e, colDetPlayerEdge({s.x, s.z},{e.x, e.z}) ? RED : GREEN);
	// 		DrawLine3D(e,t, colDetPlayerEdge({e.x, e.z},{t.x, t.z}) ? RED : GREEN);
	// 		DrawLine3D(t,s, colDetPlayerEdge({t.x, t.z},{s.x, s.z}) ? RED : GREEN);
	// 	//}
	// }

	// // //loop throuh all axis normals
	// // for(auto axisp : normals){
	// // 	float min = INFINITY;
	// // 	float max = -INFINITY;
	// // 	float pmin = INFINITY;
	// // 	float pmax = -INFINITY;

	// // 	for(int i=0; i<ccc; i+=3){
	// // 		// projected point on normal axis
	// // 		auto q = Vector2DotProduct( {cc.vertices[i+0],cc.vertices[i+2]}, axisp );
	// // 		min = std::min(min,q);
	// // 		max = std::max(min,q);

	// // 		auto pq = Vector2DotProduct( {ppos.x,ppos.y}, axisp );
	// // 		pmin = std::min(pmin,pq);
	// // 		pmax = std::max(pmin,pq);
	// // 	}


	// // }

	return false;
}
