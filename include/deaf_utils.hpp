#pragma once
#include <chrono>
#include "raylib.h"
#include "raymath.h"

struct ScopedTimer{
	std::chrono::high_resolution_clock::time_point start,end; //start and end points for timer
	std::chrono::nanoseconds* result; //store result in this address

	//start timer and specify result destination, calculated on deletion
	ScopedTimer(std::chrono::nanoseconds* result){
		this->result = result;
		start = std::chrono::high_resolution_clock::now();
	}
	~ScopedTimer(){
		end = std::chrono::high_resolution_clock::now();
		*result = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);
	}

};

inline Color hueToHSV(int hue){
    if(hue == 0) return BLACK;

	hue &= 127;
	static const int uHueRed = 18;
	static const int uHueOra = 26;
	static const int uHueYel = 36;
	static const int uHueGrn = 54;
	static const int uHueCyn = 72;
	static const int uHueBlu = 90;
	static const int uHuePur = 108;
	static const int uHueWht = 127;
	float s = 1.0f;
	float v = 1.0f;
	float h = 290.f * ((float)(hue-uHueRed) / (float)(uHuePur-uHueRed));
	if(hue < uHueRed) {v = (float)hue / (float)uHueRed; h = 0;} //fade from black
	if(hue > uHuePur) {s = 1.0 - (float)(hue-uHuePur) / (float)(uHueWht-uHuePur);} //fade to white 

	return ColorFromHSV(h,s,v);
}

inline bool colCircleLine(Vector2 ppos, float prr, Vector2 vs, Vector2 ve){

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
	// DrawText(STR(close_p1).c_str(),0,0,16,WHITE);
	// DrawText(STR(close_p2).c_str(),0,16,16,WHITE);
	// DrawText(STR(close_w1).c_str(),0,16*2,16,WHITE);
	// DrawText(STR(close_w2).c_str(),0,16*3,16,WHITE);
	// DrawText(STR(norm_p1).c_str(),0,16*4,16,WHITE);
	// DrawText(STR(norm_p2).c_str(),0,16*5,16,WHITE);
	// DrawText(STR(norm_w1).c_str(),0,16*6,16,WHITE);
	// DrawText(STR(norm_w2).c_str(),0,16*7,16,WHITE);
	// DrawLineV(conditionPoint({close_p1,-1.1}),conditionPoint({close_p2,-1.12}),LIME);
	// DrawLineV(conditionPoint({close_w1,-1.2}),conditionPoint({close_w2,-1.22}),ORANGE);
	// DrawLineV(conditionPoint({norm_p1,-1.8}),conditionPoint({norm_p2,-1.82}),PURPLE);
	// DrawLineV(conditionPoint({norm_w,-1.91}),conditionPoint({norm_w,-1.92}),YELLOW);
	// // if( pmax > w1 && pmin <= w) {
	//     // return true;
	// // }
	// 

	return shadow_close && shadow_norm;
}