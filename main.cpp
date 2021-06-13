#define SOL_ALL_SAFETIES_ON 1
#include <functional>
#include "sol/sol.hpp" // or #include "sol.hpp", whichever suits your needs
#include <raylib.h>
#include <chrono>
#include <iostream>

sol::state lua;

#define S_WIDTH 640
#define S_HEIGHT 480
#define G_DIV 8

std::vector<Font> fonts;
struct VElement;
struct VAction;
Vector2 DrawString(std::string str ,float x, float y, float s = 16, Color c = WHITE, float anchorX = 0.0f, float anchorY = 0.0f, Font f = fonts[0]);

std::vector<Texture2D> textures_in_script;
std::vector<std::string> chain;
std::string current_script;
std::vector<VAction*> actions;
std::vector<VElement*> elements;
std::function<void(void)> onFrame = nullptr;
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

std::vector<std::string> logs;
std::vector<std::string> commands;
std::string command;
int cmd_index = 0;
//#define LOG(X) std::cout << CON_YELLOW << "[Info] " << CON_NORM << X << std::endl; logs.push_back("[Info] " + std::string(X));
//#define ERROR(X) std::cout << CON_RED << "[**Error**] " << X << CON_NORM << std::endl; logs.push_back("[Error]" + std::string(X));
#define LOG(X) logs.push_back("[Info] " + std::string(X));
#define ERROR(X) logs.push_back("[Error]" + std::string(X));
#define PRINT(X) std::cout << X << std::endl;

void execCommand(){
	try{
		lua.script(command);
		//LOG("{"+command+"}");
	}
	catch(std::exception ex){
		ERROR("Oops - invalid command!");
	}	
	commands.push_back(command);
	command = "";
	cmd_index = commands.size();
}


struct VAction{
	fpstime time_to_take;
	fpstime time_current;
	float exec_ratio;
	std::string name;
	sol::function action;
	sol::function completion;
};

struct VElement{
	float x = 0;
	float y = 0;
	float w = 16;
	float h = 16;
	float a_x = 0.5f; //anchor points
	float a_y = 0.5f;
	
	Color col = WHITE;
	
	bool bound_box = false;
	bool visible = true;
	
	VElement(){};
	virtual ~VElement(){};
	
	void drawBoundBox(){
		float xx = x - w*a_x;
		float yy = y - h*a_y;
		bool hover = (CheckCollisionPointRec(GetMousePosition(),{xx,yy,w,h}));
		if(hover and IsMouseButtonPressed(0) and IsKeyDown(KEY_LEFT_SHIFT) and not in_console) bound_box = not bound_box;
		
		if(bound_box){
			DrawRectangleLines(xx,yy,w,h,(hover ? RED : WHITE));
			DrawRectangleLines(xx+1,yy+1,w-2,h-2,RED);
			DrawLine(x,yy-10,x,yy+h+10,PURPLE);
			DrawLine(xx-10,y,xx+w+10,y,BLUE);	
		}
	}
	
	void update(){
		if(visible) draw();
		drawBoundBox();
	}
	
	virtual void draw(){
		if(w == 0 or h == 0) return;
	
		float xx = x - w*a_x;
		float yy = y - h*a_y;
		
		DrawRectangle(xx,yy, w, h, col);
	};
};

struct VImage : public VElement {
	Texture2D tex;
	int tmx, tmy = 1;
	int tx, ty = 0;
	
	virtual ~VImage() {
		UnloadTexture(tex);
	}

	void SetTexture(Texture2D t){
		tex = t;
		w = tex.width;
		h = tex.height;
		tmx = 1;
	 	tmy = 1;
	 	tx = 0;
	 	ty = 0;
	}

	void draw() override {
		VElement::draw();
		//if(tex){
			float sw = tex.width/tmx;
			float sh = tex.height/tmy;
			float sx = sw*tx;
			float sy = sw*ty;
		
			DrawTexturePro(tex,
				Rectangle{sx,sy,sw,sh},
				Rectangle{x - w*a_x, y - h*a_y, w, h},
				Vector2{0,0},0,WHITE);
	
		
		//}
	};
};

struct VLabel : public VElement{

	virtual ~VLabel(){};
	
	std::string text;
	int size = 16;
	int font = 0;
	
	void draw() override {
		if(w == 0 or h == 0) return;
		if(text.size() == 0) return;
		
		w = MeasureText(text.c_str(),size);
		h = size;
		
		float xx = x - w*a_x;
		float yy = y - h*a_y;
		if(font < 0 )font = 0;
		if(font >= fonts.size()) font = fonts.size()-1;
		DrawTextEx(fonts[font], text.c_str(), {xx,yy}, size, 0 , WHITE);
	};
};

struct VButton : public VElement {
	
	virtual ~VButton(){};
	bool selected = false;
	std::function<void(void)> onPress = nullptr;
	
	void draw() override {	
		float xx = x - w*a_x;
		float yy = y - h*a_y;	
		Rectangle r = {xx,yy,w,h};
		DrawRectangleLines(xx,yy,w,h,col);
		if(CheckCollisionPointRec(GetMousePosition(),r)){
			DrawRectangle(xx+2,yy+2,w-4,h-4,col);
			if(IsMouseButtonPressed(0) and onPress){
				onPress();
			}
		}
	}
	
	//void onPress();
};

struct VTimer : public VElement {

	float progress = 0.0f;
	bool invert = false;
	bool circle = true;
	bool border = true;
	
	void draw() override {
		float p = progress;
		if(p > 1.0) p = 1.0;
		if(p < 0.0) p = 0.0;
		
		float xx = x - w*a_x;
		float yy = y - h*a_y;
		
		if(border)DrawRectangleLines(xx,yy,w,h,col);
		
		if(circle){
				BeginScissorMode(xx,yy,w,h);
				DrawCircleSector({xx + w/2,yy + h/2}, std::max(w,h), 180, (invert ? -180 : 180) - 360*progress, 32, col);
				EndScissorMode();
		}
		else{
			if(not invert)
			DrawRectangle(xx,yy,w*p,h,col);
			else
			DrawRectangle(xx + w-(w*p),yy,(w*p),h,col);
		}
		
	}
};

void script();
void script_init(){
	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::math);

	auto elem = lua.new_usertype<VElement>("VElement");
	elem["x"] = &VElement::x;
	elem["y"] = &VElement::y;
	elem["w"] = &VElement::w;
	elem["h"] = &VElement::h;
	elem["a_x"] = &VElement::a_x;
	elem["a_y"] = &VElement::a_y;
	elem["r"] = sol::property([](VElement* v){return v->col.r;}, [](VElement* v, float r){v->col.r = r;});
	elem["g"] = sol::property([](VElement* v){return v->col.g;}, [](VElement* v, float g){v->col.g = g;});
	elem["b"] = sol::property([](VElement* v){return v->col.b;}, [](VElement* v, float b){v->col.b = b;});
	elem["a"] = sol::property([](VElement* v){return v->col.a;}, [](VElement* v, float a){v->col.a = a;});
	elem["hue"] = sol::property([](VElement* v){return ColorToHSV(v->col).x;}, [](VElement* v, float h){v->col = ColorFromHSV(h,1.0,1.0);});
	
	auto lbl = lua.new_usertype<VLabel>("VLabel",sol::base_classes, sol::bases<VElement>());
	lbl["text"] = &VLabel::text;
	lbl["size"] = &VLabel::size;
	lbl["font_idx"] = &VLabel::font;

	
	lua.new_usertype<Texture2D>("VTex");
	auto image = lua.new_usertype<VImage>("VImage",sol::base_classes, sol::bases<VElement>());
	image["tex"] = sol::property(&VImage::SetTexture);
	image["tiles_count_x"] = &VImage::tmx;
	image["tiles_count_y"] = &VImage::tmy;
	image["tile_x"] = &VImage::tx;
	image["tile_y"] = &VImage::ty;
	
	auto timer = lua.new_usertype<VTimer>("VTimer",sol::base_classes, sol::bases<VElement>());
	timer["progress"] = &VTimer::progress;
	timer["circle"] = &VTimer::circle;
	timer["invert"] = &VTimer::invert;
	
	lua.new_usertype<VButton>("VButton",sol::base_classes, sol::bases<VElement>());
	
	lua["CreateTexture"] = [](std::string t) -> Texture2D {
		auto tex = LoadTexture(t.c_str());
		textures_in_script.push_back(tex);
		return tex;
	};
	
	lua["AddVLabel"] = [](std::string s) -> VLabel* { 
		auto l = new VLabel();
		l->text = s;
		elements.push_back(l);
		return l;
	};
	lua["AddVButton"] = [](sol::function f) -> VButton* { 
		auto l = new VButton();
		l->onPress = f;
		elements.push_back(l);
		return l;
	};
	lua["AddVImage"] = []() -> VImage* { 
		auto l = new VImage();
		elements.push_back(l);
		return l;
	};
	lua["AddVTimer"] = []() -> VTimer* { 
		auto l = new VTimer();
		elements.push_back(l);
		return l;
	};
	
	lua["OffsetVElement"] = [](VElement* v, float x, float y){
		if(v == nullptr) {
			ERROR("offsetting invalid object");
			return;
		}
		v->x += x;
		v->y += y;
	};
	
	lua["GridVElement"] = [](VElement* v, float gx, float gy){
		if(v == nullptr) {
			ERROR("placing invalid object");
			return;
		}
		float x = gx*(float(S_WIDTH)/float(G_DIV));
		float y = gy*(float(S_HEIGHT)/float(G_DIV));
		v->x = x;
		v->y = y;
	};
	lua["PlaceVElement"] = [](VElement* v, float x, float y){
		if(v == nullptr) {
			ERROR("placing invalid object");
			return;
		}
		v->x = x;
		v->y = y;
	};
	
	lua["DebugVElement"] = [](VElement* v){
		if(v == nullptr) {
			ERROR("invalid object");
			return;
		}
		
		v->bound_box = not v->bound_box;
	};
	
	
	lua["CancelVAction"] = [](std::string withName){
		actions.erase(std::remove_if(
			actions.begin(), 
            actions.end(),
            [=](auto a){
            	if(a->name == withName){
					return true;
            	}
            	return false;
            }),
            actions.end()
        );
	};
	
	lua["AddVAction"] = [](sol::table ac){
		
		auto a = new VAction();
		a->time_to_take = std::chrono::duration_cast<fpstime>(std::chrono::milliseconds{ac["duration"]});
		a->time_current = fpstime{0};
		a->action = ac["action"];
		a->completion = ac["completion"];
		a->exec_ratio = 0.0f;
		
		sol::optional<std::string> n = ac["name"];
		if(n == sol::nullopt) a->name = "default";
		else a->name = ac["name"];
		
		if(a->name != "default" and ac["singleton"] == true){
			lua["CancelVAction"](a->name);
		}
		
		actions.push_back(a);
	};
	
	lua["Message"] = [](std::string msg){
		message_text = msg;
		message_timer = std::chrono::duration_cast<fpstime>(std::chrono::seconds{2});
	};
	
	lua["Grid"] = [](bool state){
		layout_grid = state;
	};
	lua["Actions"] = [](bool state){
		actions_view = state;
	};
	lua["Bench"] = [](bool state){
		bench_view = state;
	};
	lua["Chain"] = [](bool state){
		chain_view = state;
	};
	
	lua["Present"] = [](std::string scr){
		current_script = scr;
		chain.push_back(scr);
		reload = true;
	};
	lua["Dismiss"] = [](){
		if(chain.size() > 1) chain.pop_back();
		current_script = chain.back();
		reload = true;
	};
	
	
	lua["LOG"] = [](sol::object o){
		if(o.get_type() == sol::type::number){
			LOG(std::to_string(float(o.as<float>())));
		}
		else if(o.is<std::string>()){
			LOG(o.as<std::string>());
		}
	};
}
void script(){
	onFrame = nullptr;
	for(auto e : elements) delete e;
	for(auto a : actions) delete a;
	elements.clear();
	actions.clear();
	for(auto t: textures_in_script){
		UnloadTexture(t);
	}
	textures_in_script.clear();
	
	try{
		lua["onFrame"] = nullptr;
		auto sc = lua.load_file(current_script);
		auto res = sc();
		script_error = not res.valid();
		onFrame = lua["onFrame"];
		PRINT("script Loaded");
	}
	catch(std::exception ex){
		script_error = true;
		PRINT("script error");
	}
	
	try{
		lua["onLoad"]();
		PRINT("script -> onLoad()");
	}
	catch(std::exception ex){
		script_error = true;
		PRINT("onLoad error");
	}
	
}
void check_script(){
	if((IsKeyPressed(KEY_R) and IsKeyDown(KEY_LEFT_SHIFT) and not in_console) or reload){
		script();
		reload = false;
	}		
}
void do_actions(){
	auto timer_actions = new ScopedTimer(&bench_actions);
	if(actions_view) DrawString("Actions:",0,0,16,GRAY);
	int yy = 1;
	for (auto a : actions){
		a->action(std::chrono::duration_cast<std::chrono::milliseconds>(a->time_current).count());
		a->time_current += fpstime{1};
		a->exec_ratio = float(a->time_current.count()) / float(a->time_to_take.count());
		
		if(actions_view)
		DrawString("[" + std::to_string(int(a->exec_ratio*100)) + "%] -> " + a->name, 0, 16*yy++, 16, GRAY);
	}
	actions.erase(std::remove_if(
		actions.begin(), 
		actions.end(),
		[](auto a){
			if(a->time_current >= a->time_to_take){
				a->completion();
				return true;
			}
			return false;
		}),
		actions.end()
	);
	delete timer_actions;
}

void do_elements(){
	auto timer_elem = new ScopedTimer(&bench_elements);
	for(auto e : elements){
		e->update();
	}
	delete timer_elem;
}

void do_grid(){
	if(layout_grid){
		const float dx = float(S_WIDTH)/float(G_DIV);
		const float dy = float(S_HEIGHT)/float(G_DIV);

		for(int y=0; y<G_DIV; y++){
			for(int x=0; x<G_DIV; x++){
				DrawLine(0,y*dy,S_WIDTH,y*dy,GRAY);
				DrawLine(x*dx,0,x*dx,S_HEIGHT,GRAY);
			}
		}
	
		auto m = GetMousePosition();
		float gx = m.x/(float(S_WIDTH)/float(G_DIV));
		float gy = m.y/(float(S_HEIGHT)/float(G_DIV));
		std::string coords;
		coords += std::to_string(gx);
		coords += ",";
		coords += std::to_string(gy);
		coords += '\n';
		coords += std::to_string(m.x);
		coords += ",";
		coords += std::to_string(m.y);
	
		auto b = DrawString(coords,m.x,m.y,16,GRAY,0.0,1.0);
		DrawRectangleLines(std::floor(gx)*dx,std::floor(gy)*dy,dx,dy,YELLOW);
	}
}

void do_console(){
	
	if(in_console){
		int c = GetCharPressed();

		if(IsKeyPressed(KEY_BACKSPACE) and command.size()){
			command.pop_back();
		}
		else if(IsKeyPressed(KEY_ENTER)){
			execCommand();
		}
		else if(IsKeyPressed(KEY_UP)){
			if(cmd_index > 0) cmd_index--;
			command = commands[cmd_index];
		}
		else if(IsKeyPressed(KEY_DOWN)){
			cmd_index++;
			if(cmd_index >= commands.size()){
				command = "";
				cmd_index = commands.size();
			}
			else
			command = commands[cmd_index];
		}
		else if(c){
			if(c != '`')
			command += c;
		}

		DrawRectangle(16,0,S_WIDTH-32,S_HEIGHT-16,{0,0,0,160});
		DrawRectangleLines(16,0,S_WIDTH-32,16,WHITE);
		DrawRectangleLines(16,16,S_WIDTH-32,S_HEIGHT-32,WHITE);

		DrawString("->" + command,16,0);
		int s = logs.size()-27;
		if(s < 0)s = 0;
		int y = 1;
		for(int i=s; i<logs.size(); i++){
			auto line = logs[i];
			DrawString(">" + line,16,16*y++);
		}
	}
	if(IsKeyPressed(KEY_GRAVE)) in_console = not in_console;

}

void screen(){
	while(not WindowShouldClose()){
		
		BeginDrawing();
		ClearBackground(BLACK);
		
		auto timer_fps = new ScopedTimer(&bench_fps);
		auto timer_frame = new ScopedTimer(&bench_frame);
		if(onFrame) onFrame();
		delete timer_frame;
		
		do_elements();
		
		do_actions();
		
		if(not message_text.empty() and message_timer.count() > 0){
			DrawRectangle(0,0,640,64,{255,255,255,64});
			DrawString(message_text,320,32,32,WHITE,0.5,0.5);
			message_timer -= fpstime{1};
		}
		
		delete timer_fps;
		
		do_grid();
		
		if(script_error){
			DrawRectangle(0,0,640,32,GRAY);
			DrawString("Error in script",0,0,32);
		}
		if(bench_view){
			std::chrono::nanoseconds total = std::chrono::duration_cast<std::chrono::nanoseconds>(fpstime{1});
			float ratio_fps = float(bench_fps.count()) / float(total.count());
			float ratio_frame = float(bench_frame.count()) / float(total.count());
			float ratio_actions = float(bench_actions.count()) / float(total.count());
			float ratio_elements = float(bench_elements.count()) / float(total.count());
			float ratio_total = float(total.count());
			
			DrawRectangleLines(524,8,100.0f,8,GREEN);
			DrawRectangle(524,8,100.0f*ratio_fps,8,GREEN);
			
			DrawRectangleLines(524,16,100.0f,8,WHITE);
			DrawRectangle(524,16,100.0f*ratio_frame,8,PURPLE);
			DrawRectangle(524+100.0f*ratio_frame,16,100.0f*ratio_actions,8,PURPLE);
			DrawRectangle(524+100.0f*ratio_actions,16,100.0f*ratio_elements,8,SKYBLUE);
			DrawString("[Fps] " + std::to_string(100.0f*ratio_fps)+"%",500,32,16,GRAY);
			DrawString("[Frm] " + std::to_string(100.0f*ratio_frame)+"%",500,48,16,GRAY);
			DrawString("[Act] " + std::to_string(100.0f*ratio_actions)+"%",500,64,16,GRAY);
			DrawString("[Elm] " + std::to_string(100.0f*ratio_elements)+"%",500,80,16,GRAY);
		}
		
		if(chain_view){
			int y = 0;
			int x = 0;
			for(auto s : chain){
				if(x==chain.size()-1) s += "<";
				DrawString(s,16*x++,S_HEIGHT-16*y++,16,WHITE,0.0,1.0);
			}
		}
		
		do_console();
		
		
		EndDrawing();
		
		check_script();
	}
	CloseWindow();
}

Vector2 DrawString(std::string str ,float x, float y, float s, Color c, float anchorX, float anchorY, Font f){
	Vector2 bounds = MeasureTextEx(f,str.c_str(),s,0);
	Vector2 pos = {x - bounds.x*anchorX, y- bounds.y*anchorY};
	DrawTextEx(f,str.c_str(), pos, s, 0, c);
	return bounds;
}

	
	
void init(){
	InitWindow(640,480,"scripter");
	SetTargetFPS(30);
	auto fixedsys = LoadFont("fixedsys.ttf");
	auto consolas = LoadFont("Consolas.ttf");
	fonts.push_back(fixedsys);
	fonts.push_back(consolas);
	commands.push_back("");
	command = "";
}

int main(int argc, char* argv[]) {
	
	current_script = "scripts/main.lua";
	chain.push_back(current_script);
	
	init();
	script_init();
	script();
	screen();	
	
	return 0;
}