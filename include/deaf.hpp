#pragma once

#define VER "S/W/T//"
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

#include "raylib.h"

inline unsigned int S_WIDTH = 800;
inline unsigned int S_HEIGHT = 600;
inline unsigned int S_HEIGHT_T = 680;
inline int g_div = 8;


#define STR(X) std::to_string(X)
#define LOG(X) std::cout << "[Info] " << X << std::endl; 
#define ERROR(X) std::cout << "[**Error**] " << X << std::endl;
#define IS_SHIFT_DOWN IsKeyDown(KEY_LEFT_SHIFT)