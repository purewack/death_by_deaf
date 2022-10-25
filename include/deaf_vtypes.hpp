#pragma once
#include "raylib.h"

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


struct VImage : public VElement {
	Texture2D tex;
	int tmx, tmy = 1;
	int tx, ty = 0;
	
	virtual ~VImage();

	void SetTexture(Texture2D t);

	void draw() override;
};

struct VObject : public VElement {
	float z = 0.f;
	float a_z = 0.f;
	float rot_x = 0.f;
	float scale = 1.f;
	Model model;
	void draw() override;

	VObject(std::string path);
	virtual ~VObject();
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

struct VUnitButton : public VButton {
	virtual ~VUnitButton(){};
    int note = 0;
    int type = MidiBytes::on;
    int v_hue = 0;
    int v_hue_old = 0;
};

struct VTimer : public VElement {

	float progress = 0.0f;
	bool invert = false;
	bool circle = true;
	bool border = true;
	
	void draw() override;
};

inline std::vector<Font> fonts;
inline Font ui_font;
Vector2 DrawString(std::string str ,float x, float y, float s = 16, Color c = WHITE, float anchorX = 0.0f, float anchorY = 0.0f, Font f = ui_font);
void onSceneVideoFrame();