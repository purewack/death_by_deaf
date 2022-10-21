#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <functional>
#include <atomic>
#include <cstdlib>

#include "raylib.h"
#include "audio.hpp"
#include "sol.hpp" 

inline std::string sesh;

inline unsigned int S_WIDTH = 800;
inline unsigned int S_HEIGHT = 600;
inline unsigned int S_HEIGHT_T = 680;
inline int g_div = 8;

inline sol::state lua;
inline std::mutex mtx_fps;

struct VAction;
struct VSequence;
struct VElement;
struct VImage;
struct VLabel;
struct VButton;
struct VTimer;


inline std::vector<Font> fonts;
inline Font ui_font;
Vector2 DrawString(std::string str ,float x, float y, float s = 16, Color c = WHITE, float anchorX = 0.0f, float anchorY = 0.0f, Font f = ui_font);

inline std::vector<Texture2D> textures_in_script;
inline std::vector<std::string> chain;
inline std::vector<std::string> mevents;
inline std::string current_script;
inline std::vector<VSequence*> actions;
inline std::vector<VElement*> elements;
inline std::function<void(void)> onFrame = nullptr;
inline std::function<void(void)> onUIFrame = nullptr;
using fpstime = std::chrono::duration<int64_t, std::ratio<1,30>>;

inline std::string message_text;
inline fpstime message_timer;

inline bool script_error = false;
inline bool layout_grid = false;
inline bool actions_view = false;
inline bool bench_view = false;
inline bool chain_view = false;
inline bool audio_que_view = false;
inline bool midi_view = false;
inline bool in_console = false;
inline bool reload = false;
inline bool shift_down = false;

inline std::chrono::high_resolution_clock::time_point time_ac_now,time_ac_old; //start and end points for timer
inline std::chrono::milliseconds time_ac_dur;

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
inline std::chrono::nanoseconds bench_fps;
inline std::chrono::nanoseconds bench_frame;
inline std::chrono::nanoseconds bench_actions;
inline std::chrono::nanoseconds bench_elements;
inline std::chrono::nanoseconds bench_dsp;
inline std::chrono::nanoseconds max_dsp;

inline std::vector<std::string> commands;
inline std::string command;
inline int cmd_index = 0;

#define STR(X) std::to_string(X)
#define LOG(X) std::cout << "[Info] " << X << std::endl; 
#define ERROR(X) std::cout << "[**Error**] " << X << std::endl;
#define IS_SHIFT_DOWN IsKeyDown(KEY_LEFT_SHIFT)

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
