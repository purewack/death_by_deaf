#pragma once
#include <string>
#include "raylib.h"
#include "rlgl.h"
inline sol::state lua;
inline sol::table lua_system;
inline sol::table lua_visuals;
inline sol::table lua_control;
inline sol::table lua_audio;
void lua_init();
void lua_Vbind();

struct VAction{
	std::string name;
	std::chrono::milliseconds time_to_take;
    sol::function action;
};

struct VSequence{
    std::string name;
    int count;
    std::vector<VAction*> actions;
	std::chrono::milliseconds time_current;
	float exec_ratio;
    ~VSequence();
};

struct VElement{
    unsigned long id = 0;//(unsigned long)this;
	float x = 0;
	float y = 0;
	float w = 16;
	float h = 16;
	float a_x = 0.5f; //anchor points
	float a_y = 0.5f;
        
	std::string tag = "default";
	Color col = WHITE;
	
	bool bound_box = false;
	bool visible = true;
    bool focus = false;
	
	VElement(){};
	virtual ~VElement(){};
	
	void drawBoundBox();
	
	void update();
	
	virtual void draw();
};

struct VLabel : public VElement{

	virtual ~VLabel(){};
	
	std::string text;
	int size = 16;
	int font = 0;
	
	void draw() override;
};

struct VButton : public VElement {
	
	VButton(){col = BLACK;}
	virtual ~VButton(){};
	bool state = false;
	bool selected = false;
	std::function<void(void)> onPress = nullptr;
	std::function<void(void)> onRelease = nullptr;
	
	void draw() override;
	//void onPress();
};

struct VTexture {
	Texture2D texture;
	std::string filename;
};

struct VModel {
	Model mesh;
	std::string filename;
};

struct VImage : public VElement {
	VTexture tex;
	int tmx, tmy = 1;
	int tx, ty = 0;
	
	VImage(std::string tex);
	virtual ~VImage(){};

	void SetTexture(std::string t);

	void draw() override;
};

struct VObject : public VElement {
	float z = 0.f;
	//anchor z
	float a_z = 0.f;

	//axis of rotation
	float ax_x = 0.f;
	float ax_z = 0.f;
	float ax_y = 0.f;
	
	//axis angle
	float ax_angle = 0.f;

	float scale = 1.f;
	VModel model;
	void draw() override;

	VObject(std::string path);
	virtual ~VObject(){};
	void SetModel(std::string filename);
};

struct VPlayer
{
	Camera3D cam;
	Vector2 rot;
	Vector2 mpos;
	Vector2 mpos_new;
	bool active = true;
};


struct VTrigger : public VObject {

	float sx = 1.f;
	float sy = 1.f;
	float sz = 1.f;

	VTrigger();
	virtual ~VTrigger();

	bool contact_old;
	bool contact_now;
	std::function<void(void)> onContactBegin;
	std::function<void(void)> onContactEnd;

	void draw() override;
};

inline VPlayer puppet = {0};
inline std::vector<Font> fonts;
inline Font ui_font;
Vector2 DrawString(std::string str ,float x, float y, float s = 16, Color c = WHITE, float anchorX = 0.0f, float anchorY = 0.0f, Font f = ui_font);
void onSceneVideoFrame();