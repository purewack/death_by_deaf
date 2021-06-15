#define SOL_ALL_SAFETIES_ON 1
#include <functional>
#include "sol/sol.hpp" // or #include "sol.hpp", whichever suits your needs
#include <raylib.h>
#include "types.hpp"

void makeRoot(const char* argv){
	auto name = std::string("lua_motif");
	
	char* p = (char*)malloc(sizeof(char)*1024);
	getcwd(p,1024);
	root = std::string(p) + "/";
	std::string arg0 = "";
	arg0 += std::string(argv);
	arg0 = arg0.substr(0,arg0.size()-name.size());
	
	if(arg0[0] == '/') root = "";
	root += arg0;
}

void execCommand(){
	try{
		lua.script(command);
		//LOG("{"+command+"}");
	}
	catch(std::exception ex){
		ERROR("Invalid command : " + command);
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
	bool togglable = false;
	bool toggled = false;
	std::function<void(void)> onPress = nullptr;
	
	void draw() override {	
		float xx = x - w*a_x;
		float yy = y - h*a_y;	
		Rectangle r = {xx,yy,w,h};
		DrawRectangleLines(xx,yy,w,h,col);
		
		if(togglable){
			if(toggled) DrawRectangle(xx+2,yy+2,w-4,h-4,col);
			if(CheckCollisionPointRec(GetMousePosition(),r) and IsMouseButtonPressed(0) and onPress){
				if(not toggled) onPress();
				toggled = not toggled;
			}
		}
		else if(CheckCollisionPointRec(GetMousePosition(),r)){
			DrawRectangle(xx+2,yy+2,w-4,h-4,col);
			if(IsMouseButtonPressed(0) and onPress){
				onPress();
			}
		}
		
		if(selected)
		DrawRectangleLines(xx-2,yy-2,w+4,h+4,col);
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
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::io);
	lua.open_libraries(sol::lib::math);
	lua.require_file("json",root + "utils/json.lua");

	auto elem = lua.new_usertype<VElement>("VElement");
	elem["x"] = &VElement::x;
	elem["y"] = &VElement::y;
	elem["w"] = &VElement::w;
	elem["h"] = &VElement::h;
	elem["ax"] = &VElement::a_x;
	elem["ay"] = &VElement::a_y;
	elem["gx"] = sol::property([](VElement* v, float gx){v->x = gx*(float(S_WIDTH)/float(g_div));});
	elem["gy"] = sol::property([](VElement* v, float gy){v->y = gy*(float(S_HEIGHT)/float(g_div));});
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
	timer["circular"] = &VTimer::circle;
	timer["invert"] = &VTimer::invert;
	
	auto btn = lua.new_usertype<VButton>("VButton",sol::base_classes, sol::bases<VElement>());
	btn["toggle"] = &VButton::togglable;
	btn["selected"] = &VButton::selected;
	btn["action"] = sol::property([](VButton* b, std::function<void(void)> f){b->onPress = f;});
	
	lua["CreateTexture"] = [](std::string t) -> Texture2D {
		auto tex = LoadTexture((root+t).c_str());
		textures_in_script.push_back(tex);
		return tex;
	};
	
	lua["AddVElement"] = []() -> VElement* { 
		auto l = new VElement();
		elements.push_back(l);
		return l;
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
	
	lua["GridDiv"] = [](int d){
		g_div = d;
	};
	lua["GridW"] = []() -> int{
		return (S_WIDTH)/g_div;
	};
	lua["GridH"] = []() -> int{
		return (S_HEIGHT)/g_div;
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
	
	//load builder UI
	try{
		auto sc = lua.load_file(root + "utils/ui.lua");
		auto result = sc();
		if (result.valid()) {
			LOG("ui success");
		}
		else {
			// Call failed
			sol::error err = result;
			std::string what = err.what();
			std::cout << "call failed, sol::error::what() is " << what << std::endl;
			// 'what' Should read 
			// "Handled this message: negative number detected"
		}
	}
	catch(std::exception ex){
		LOG("ui error");
	}
	
	//load screen script
	try{
		lua["onFrame"] = nullptr;
		auto sc = lua.load_file(root + current_script);
		auto result = sc();
		script_error = not result.valid();
		if (result.valid()) {
			onFrame = lua["onFrame"];
			LOG("script success");
		}
		else {
			// Call failed
			sol::error err = result;
			std::string what = err.what();
			std::cout << "call failed, sol::error::what() is " << what << std::endl;
			// 'what' Should read 
			// "Handled this message: negative number detected"
		}
	}
	catch(std::exception ex){
		script_error = true;
		LOG("script error");
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
		const float dx = float(S_WIDTH)/float(g_div);
		const float dy = float(S_HEIGHT)/float(g_div);

		for(int y=0; y<g_div; y++){
			for(int x=0; x<g_div; x++){
				DrawLine(0,y*dy,S_WIDTH,y*dy,GRAY);
				DrawLine(x*dx,0,x*dx,S_HEIGHT,GRAY);
			}
		}
	
		auto m = GetMousePosition();
		float gx = m.x/(float(S_WIDTH)/float(g_div));
		float gy = m.y/(float(S_HEIGHT)/float(g_div));
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
	}

	DrawRectangle(16,S_HEIGHT,S_WIDTH-32,16,(in_console ? DARKGRAY : BLACK));
	DrawString("->" + command,16,S_HEIGHT);
	if(not in_console)DrawRectangleLines(16,S_HEIGHT,S_WIDTH-32,16,DARKGRAY);	
	
	if(IsKeyPressed(KEY_GRAVE)) in_console = not in_console;

}

void screen(){
	while(not WindowShouldClose()){
		
		BeginDrawing();
		ClearBackground(BLACK);
	
		auto timer_fps = new ScopedTimer(&bench_fps);
		auto timer_frame = new ScopedTimer(&bench_frame);
		mtx_lua.lock();
		if(onFrame) onFrame();
		mtx_lua.unlock();
		delete timer_frame;
		
		mtx_lua.lock();
		do_actions();
		mtx_lua.unlock();
		
		do_elements();
		
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
		
		mtx_lua.lock();
		check_script();
		mtx_lua.unlock();
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
	InitWindow(640,580,"scripter");
	SetTargetFPS(30);
	auto fixedsys = LoadFont((root + "fixedsys.ttf").c_str());
	auto consolas = LoadFont((root + "Consolas.ttf").c_str());
	fonts.push_back(fixedsys);
	fonts.push_back(consolas);
	commands.push_back("");
	command = "";
}

int main(int argc, char* argv[]) {
	
	makeRoot(argv[0]);
		
	current_script = "scripts/main.lua";
	chain.push_back(current_script);
	
	init();
	script_init();
	script();
	
	try{
		lua["onInit"]();
	}
	catch(std::exception ex){
		script_error = true;
		LOG("onLoad error");
	}
	
	static bool running = true;
	std::thread t([=](){
		while(running){
			pollMidi();
			usleep(5000);
		}
	});
	
	screen();	
	
	running = false;
	t.join();
	
	return 0;
}



void pollMidi(){
	//connection management
	unsigned int nDevicesCount = scanner.getPortCount();
	if(nDevicesCount != devicesCount){
		LOG((nDevicesCount > devicesCount ? "Device connect" : "Device disconnect"));
		for( RtMidiIn* d : devices ){
			d->closePort();
		}
		devices.clear();
		for (int i=0; i<nDevicesCount; i++) {
			try {
				devices.push_back(new RtMidiIn());
				devices.back()->openPort(i);
			}
			catch ( RtMidiError &error ) {
				error.printMessage();
			}
		}
		devicesCount = nDevicesCount;
	}
	

	//scan event per device
	for(RtMidiIn* input : devices){
		for(int d=0; d<nDevicesCount; d++){
			input->getMessage( &rawmidi );
			int nBytes = rawmidi.size();

			unsigned char bytes[3];
			int n=0;
			MidiData m;

			for (int i=0; i<nBytes; i++ ){
				bytes[n] = rawmidi[i];
				n++;
				if(n==3){
					n = 0;
					
					m.parse(bytes);
					LOG(m.print());
					
					checkEvent(&m);
				}
			}
		}
	}
}

void checkEvent(MidiData* m){
	mtx_lua.lock();
		auto mappings = lua["mappings"];
		auto ev = mappings[m->s][m->n];
		//if(midis == sol::type::table)
		
	mtx_lua.unlock();
}