#pragma once
#include "raylib.h"

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
