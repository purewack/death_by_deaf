#pragma once
#include <chrono>
#include "raylib.h"

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