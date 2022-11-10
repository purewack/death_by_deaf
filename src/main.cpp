#include "deaf.hpp"
#include "raymath.h"

bool colDetPlayerEdge(Vector2 vs, Vector2 ve);
bool colDetPlayer(Mesh &cc);
Vector2 player = {0,0};
float playerR = 1.1;

int ss = 6;
int dd = 0;

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

	Vector2 w1 = {1,2};
	Vector2 w2 = {-0.5,1.9};
	dd = std::min(S_WIDTH,S_HEIGHT)/ss;
	
	while(not WindowShouldClose()){
        
        BeginDrawing();
        ClearBackground(BLACK);
			
			for(int d=0; d<ss; d++){
				DrawLine(d*dd,0,d*dd,S_HEIGHT,d != ss/2 ? GRAY : RED);
				DrawLine(0,d*dd,S_WIDTH,d*dd,d != ss/2 ? GRAY : RED);
			}

			auto p1 = conditionPoint(w1);
			auto p2 = conditionPoint(w2);
			DrawLineV(p1,p2,colDetPlayerEdge(w1,w2) ? RED : YELLOW);

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
	float prr = 0.5; //player radius
	LOG("pos:");
	LOG(ppos.x);
	LOG(ppos.y);

	//find closest point to circle
	float c1 = Vector2Distance(ppos,vs);
	float c2 = Vector2Distance(ppos,ve);
	LOG("dist");
	LOG(c1);
	LOG(c2);
	Vector2 cp; //closest point
	if(c1 < c2){
		cp = vs;
		LOG("vs");
	}
	else{
		cp = ve;
		LOG("ve");
	}
	
	auto axis = Vector2Subtract(ppos,cp);
	// axis.x *= -1;
	LOG("axis");
	LOG(axis.x);
	LOG(axis.y);
	DrawLineV(conditionPoint({0}),conditionPoint(axis),GREEN);
	DrawLineV(conditionPoint({ppos}),conditionPoint(cp),LIME);

	// //axis parallel to closest point and circle center
	// auto axis = Vector2Subtract(Vector2{ve.y,ve.x},Vector2{vs.y,vs.x});
	// axis.x *= -1;
	// LOG("axis");
	// LOG(axis.x);
	// LOG(axis.y);
	
	// //axis correct normal to edge
	// auto axi2 = Vector2{-(ve.y-vs.y),(ve.x-vs.x)};
	// LOG("axi2");
	// LOG(axi2.x);
	// LOG(axi2.y);

	// projected circle points on axis of test
	
	auto p1 = Vector2DotProduct( ppos, axis )+prr;
	auto p2 = Vector2DotProduct( ppos, axis )-prr;
	// projected wall points on axis of test
	auto w1 = Vector2DotProduct( vs, axis );
	auto w2 = Vector2DotProduct( ve, axis );
	LOG("dots");
	LOG(p1);
	LOG(p2);
	LOG(w1);
	LOG(w2);

	auto pmin = std::min(p1,p2);
	auto pmax = std::max(p1,p2);
	auto wmin = std::min(w1,w2);
	auto wmax = std::max(w1,w2);
	LOG("limits");
	LOG(pmin);
	LOG(pmax);
	LOG(wmin);
	LOG(wmax);
    
	DrawText(STR(pmin).c_str(),0,0,16,WHITE);
	DrawText(STR(pmax).c_str(),0,16,16,WHITE);
	DrawText(STR(wmin).c_str(),0,16*2,16,WHITE);
	DrawText(STR(wmax).c_str(),0,16*3,16,WHITE);

	auto ppmin = conditionPoint({pmin,-3});
	auto ppmax = conditionPoint({pmax,-3});
	auto pwmin = conditionPoint({wmin,-2.6});
	auto pwmax = conditionPoint({wmax,-2.6});
	DrawLineV(ppmin,ppmax,BLUE);
	DrawLineV(pwmin,pwmax,YELLOW);
	// if( pmax > w1 && pmin <= w) {
	    // return true;
	// }

	return false;
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
