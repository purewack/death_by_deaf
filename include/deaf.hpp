#pragma once

#define VER "S/W/D/3"
#define DEBUG

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

#include "sol.hpp" 
#include "raylib.h"
#include "deaf_audio.hpp"
#include "deaf_lua.hpp"
#include "deaf_utils.hpp"

inline unsigned int TICK_TIME = 16666; //30tick / sec
inline unsigned int S_WIDTH = 800;
inline unsigned int S_HEIGHT = 600;
inline int g_div = 8;

inline std::mutex mtx_fps;

inline std::vector<VTexture> textures_in_script;
inline std::vector<VModel> models_in_script;
inline std::vector<std::string> chain; //script nav chain
inline std::vector<std::string> mevents; //midi events
inline std::string current_script;
inline std::vector<VSequence*> actions;
inline std::vector<VElement*> elements;
inline std::vector<VObject*> objects;
inline std::function<void(float)> onFrame = nullptr;
inline std::function<void(void)> onFrame3D = nullptr;
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
inline bool floor_view = false;
inline bool in_console = false;
inline bool reload = false;
inline bool shift_down = false;

inline std::chrono::high_resolution_clock::time_point time_ac_now,time_ac_old; //start and end points for timer
inline std::chrono::milliseconds time_ac_dur;

inline std::chrono::nanoseconds bench_fps;
inline std::chrono::nanoseconds bench_frame;
inline std::chrono::nanoseconds bench_actions;
inline std::chrono::nanoseconds bench_elements;
inline std::chrono::nanoseconds bench_dsp;
inline std::chrono::nanoseconds max_dsp;

inline std::vector<std::string> loglines;
inline int loglines_cursor = 0;
inline std::vector<std::string> commands;
inline std::string command;
inline int cmd_index = 0;

void printLog(std::string msg);
#define STR(X) std::to_string(X)
#define LOG(X) std::cout << "[Info] " << X << std::endl; 
#define ERROR(X) std::cout << "[**Error**] " << X << std::endl;
#define IS_SHIFT_DOWN IsKeyDown(KEY_LEFT_SHIFT)
