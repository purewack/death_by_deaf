#pragma once
#include <vector>
#include <rtmidi/RtMidi.h>
#include <raylib.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <functional>
#include <atomic>

std::string root;
std::string sesh;

#define S_WIDTH 640
#define S_HEIGHT 480
#define S_HEIGHT_T 580
int g_div = 8;

sol::state lua;
std::mutex mtx_fps;

enum MidiBytes: int{
	on = 144, //x90
	off = 128, //x80
	cc = 176, //xb0
};

std::vector<unsigned char> rawmidi;
RtMidiIn scanner;
int devicesCount = 0;
std::vector<RtMidiIn*> devices;


struct VAction;
struct VSequence;
struct VElement;
struct VImage;
struct VLabel;
struct VButton;
struct VTimer;


std::vector<Font> fonts;
Font ui_font;
Vector2 DrawString(std::string str ,float x, float y, float s = 16, Color c = WHITE, float anchorX = 0.0f, float anchorY = 0.0f, Font f = ui_font);

std::vector<Texture2D> textures_in_script;
std::vector<std::string> chain;
std::vector<std::string> mevents;
std::string current_script;
std::vector<VSequence*> actions;
std::vector<VElement*> elements;
std::function<void(void)> onFrame = nullptr;
std::function<void(void)> onUIFrame = nullptr;
using fpstime = std::chrono::duration<int64_t, std::ratio<1,30>>;

std::string message_text;
fpstime message_timer;

bool script_error = false;
bool layout_grid = false;
bool actions_view = false;
bool bench_view = false;
bool chain_view = false;
bool midi_view = false;
bool in_console = false;
bool reload = false;
bool shift_down = false;

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
std::chrono::nanoseconds bench_fps;
std::chrono::nanoseconds bench_frame;
std::chrono::nanoseconds bench_actions;
std::chrono::nanoseconds bench_elements;
std::chrono::nanoseconds bench_dsp;

std::vector<std::string> commands;
std::string command;
int cmd_index = 0;

#define LOG(X) std::cout << "[Info] " << X << std::endl; 
#define ERROR(X) std::cout << "[**Error**] " << X << std::endl;
bool sys_shift = false;
#define IS_SHIFT_DOWN IsKeyDown(KEY_LEFT_SHIFT) or sys_shift

Color hueToHSV(int hue){
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