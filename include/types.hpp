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

std::string root;
std::string sesh;

#define S_WIDTH 640
#define S_HEIGHT 480
#define S_HEIGHT_T 580
int g_div = 8;

sol::state lua;
std::mutex mtx_lua;

enum MidiBytes: int{
	on = 144, //x90
	off = 128, //x80
	cc = 176, //xb0
};
struct MidiData{
	//int w = 0; //who
	int c = 0; //channel
	int s = 0; //status
	int n = 0; //note
	int v = 0; //vel

	void parse(unsigned char* bytes){
		c = bytes[0] & 0x0F; //strip channel byte
		s = bytes[0] & 0xF0; //strip status byte
		n = bytes[1]; //note
		v = bytes[2]; //vel
		if(s==MidiBytes::on and v == 0) {s=MidiBytes::off;}
	}
	float range(float scaleMin, float scaleMax){
		return scaleMin+((float(v)/127.0)*(scaleMax-scaleMin));
	}

	bool operator==(const MidiData &m) const {
		return (c==m.c and s==m.s and n==m.n);
	}
	bool operator< (const MidiData &m) const {
		return (c<m.c  or  s<m.s  or  n<m.n);
	}
	std::string print(){
		//"{" + std::to_string(w) + "}" +
		return  "[" + std::to_string(c) + "]" + (s==MidiBytes::cc ? "CC " : "Note ") + " " + std::to_string(s) + " " + std::to_string(n) + " " + std::to_string(v);
	}
};


struct MidiMapCommand{
	std::string c = "";
	bool usesValue = false;
	float min = 0.0;
	float max = 1.0;
	bool absCC = true;
	int prevCC = 0;
};

std::vector<unsigned char> rawmidi;
RtMidiIn scanner;
int devicesCount = 0;
std::vector<RtMidiIn*> devices;

struct MidiElement{
	int id; 
	bool knob = false;
	int rot = 0;
	int rotOld = 0;
	bool inreg = 0;
	bool inregOld = 0;
	bool stopHoldTime = false;
	long timeDelta = 0;
	int hue = 0;
	static const int HOLD_TIME = 2000;
};

void pollMidi();
void checkEvent(MidiData* m);


struct VElement;
struct VAction;
struct VImage;
struct VLabel;
struct VButton;
struct VTimer;


std::vector<Font> fonts;
Vector2 DrawString(std::string str ,float x, float y, float s = 16, Color c = WHITE, float anchorX = 0.0f, float anchorY = 0.0f, Font f = fonts[0]);

std::vector<Texture2D> textures_in_script;
std::vector<std::string> chain;
std::string current_script;
std::vector<VAction*> actions;
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

#define IS_SHIFT_DOWN IsKeyDown(KEY_LEFT_SHIFT)