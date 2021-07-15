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

Vector2 DrawString(std::string str ,float x, float y, float s, Color c, float anchorX, float anchorY, Font f)
{
	Vector2 bounds = MeasureTextEx(f,str.c_str(),s,0);
	Vector2 pos = {x - bounds.x*anchorX, y- bounds.y*anchorY};
	DrawTextEx(f,str.c_str(), pos, s, 0, c);
	return bounds;
}

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
    ~VSequence(){
    	for(auto a : actions) delete a;
    	actions.clear();
    }
};

struct VElement{
    unsigned long id = (unsigned long)this;
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
	
	void drawBoundBox(){
		float xx = x - w*a_x;
		float yy = y - h*a_y;
		bool hover = (CheckCollisionPointRec(GetMousePosition(),{xx,yy,w,h}));
		if(hover and IsMouseButtonPressed(0) and IS_SHIFT_DOWN and not in_console) bound_box = not bound_box;
		hover |= focus;
        
		if(hover and IS_SHIFT_DOWN) 
		{
			DrawString(tag,x-1,y-1,16,BLACK);
			DrawString(tag,x,y);
		}
		if(bound_box){
			DrawRectangleLines(xx,yy,w,h,(hover ? RED : WHITE));
			DrawRectangleLines(xx+1,yy+1,w-2,h-2,RED);
			DrawLine(x,yy-10,x,yy+h+10,PURPLE);
			DrawLine(xx-10,y,xx+w+10,y,BLUE);	
			
			std::string pos;
			pos += std::to_string(x);
			pos += ",";
			pos += std::to_string(y);
			DrawString(pos,x,y);
		}
        if(focus){
            DrawRectangle(xx,yy,w,h,{0,255,255,128});
        }
	}
	
	void update(){
		if(visible and col.a) draw();
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
		if(font < 0 )font = 0;
		if(font >= fonts.size()) font = fonts.size()-1;
		
        auto ss = MeasureTextEx(fonts[font],text.c_str(),size,0.f);
		w = ss.x;
        h = ss.y;
		float xx = x - w*a_x;
		float yy = y - h*a_y;
		DrawTextEx(fonts[font], text.c_str(), {xx,yy}, size, 0 , WHITE);
	};
};

struct VButton : public VElement {
	
	VButton(){col = BLACK;}
	virtual ~VButton(){};
	bool state = false;
	bool selected = false;
	std::function<void(void)> onPress = nullptr;
	std::function<void(void)> onRelease = nullptr;
	
	void draw() override {	
		float xx = x - w*a_x;
		float yy = y - h*a_y;	
		Rectangle r = {xx,yy,w,h};
		DrawRectangleLines(xx,yy,w,h,WHITE);
		DrawRectangle(xx+2,yy+2,w-4,h-4,(state ? WHITE : col));
		if(selected) DrawRectangleLines(xx-2,yy-2,w+4,h+4,WHITE);
	}	
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



void lua_init()
{
    
	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::io);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::os);
	lua["S_W"] = S_WIDTH;
	lua["S_H"] = S_HEIGHT;
	lua["S_HT"] = S_HEIGHT_T;
	lua["root"] = root;
	lua.require_file("json",root + "scripts/json.lua");
	
    auto l_system = lua["system"].get_or_create<sol::table>();
    auto l_visuals = lua["visuals"].get_or_create<sol::table>();
    auto l_control = lua["control"].get_or_create<sol::table>();
    auto l_audio = lua["audio"].get_or_create<sol::table>();
    auto l_sesh = lua["sesh"].get_or_create<sol::table>();
    
	l_system["loadFont"] = [](std::string name){
    	auto f = LoadFont((root + name).c_str());
        fonts.push_back(f);
	};
    l_system["uiFont"] = [](int i){
        if(i >= 0 and i < fonts.size()) ui_font = fonts[i];
    };
	l_system["gridDiv"] = [](int d){
		g_div = d;
	};
	l_system["gridW"] = []() -> int{
		return (S_WIDTH)/g_div;
	};
	l_system["gridH"] = []() -> int{
		return (S_HEIGHT)/g_div;
	};
	l_system["grid"] = [](bool state){
		layout_grid = state;
	};
	l_system["actions"] = [](bool state){
		actions_view = state;
	};
	l_system["bench"] = [](bool state){
		bench_view = state;
	};
	l_system["chain"] = [](bool state){
		chain_view = state;
	};
	l_system["midi"] = [](bool state){
		midi_view = state;
	};
	l_system["toggleAnimation"] = [](){
		if(onFrame) onFrame = nullptr;
		else onFrame = lua["onFrame"];
	};
    l_system["push_command"] = [](std::string cmd){
        commands.push_back(cmd);
    };
    l_system["async_after"] = [=](sol::function action, int after_ms, std::string withName){  
		auto w = new VAction();
        w->time_to_take = std::chrono::milliseconds{after_ms};
		
        auto a = new VAction();
        a->time_to_take = std::chrono::milliseconds{0};
        a->action = action;
        
        auto s = new VSequence();
		s->time_current = std::chrono::milliseconds{0};
        s->exec_ratio = 0.0f;
        s->count = 1;
	    s->name = withName;
        s->actions.push_back(w);
        s->actions.push_back(a);
                  
        actions.push_back(s);
    };
    l_system["async_cancel"] = [=](std::string withName){
        l_visuals["cancelVSequence"](withName);
    };
    
    l_system["readJSON"] = [=](std::string path){
        lua["io"]["input"](path,"r");
        auto text = lua["io"]["read"]("*all");
        lua["current_json"] = lua["json"]["decode"](text);
        lua.script("io.input():close()");
    };
    l_system["writeJSON"] = [=](std::string path){        
        lua["io"]["output"](path,"w");
        auto to_write = lua["json"]["encode"](lua["current_json"]);
        lua["io"]["write"](to_write);
        lua.script("io.output():close()");
    };

    l_sesh["load"] = [](std::string path){
        
    };
    l_sesh["save"] = [](std::string path){
        
    };
    l_sesh["stopped"] = true;
    l_sesh["recording"] = false;
    l_sesh["playing"] = false;

    l_control["shift"] = (int)0;
    l_control["ev_vel"] = (int)0;
    l_control["ev_note_on"] = MidiBytes::on;
    l_control["ev_note_off"] = MidiBytes::off;
    l_control["ev_note_cc"] = MidiBytes::cc;
    l_control["map"].get_or_create<sol::table>();
    l_control["map"][0x90].get_or_create<sol::table>();
    l_control["map"][0x80].get_or_create<sol::table>();
    l_control["map"][0xA0].get_or_create<sol::table>();
    l_control["unitmap"].get_or_create<sol::table>();
    l_control["unitmap"][0x90].get_or_create<sol::table>();
    l_control["unitmap"][0x80].get_or_create<sol::table>();
    l_control["unitmap"][0xA0].get_or_create<sol::table>();
    l_control["units"].get_or_create<sol::table>();
    l_control["focus_idx"] = (int)0;
    l_control["navigables"].get_or_create<sol::table>();
    l_control["navigate"] = [=](){
        sol::table navigables = l_control["navigables"];
        if(navigables == sol::nil) return;
        
        unsigned int focal = 0;
        unsigned int count = 0;
        for (const auto& key_value_pair : navigables ) {
             sol::object key = key_value_pair.first;
             sol::object value = key_value_pair.second;
             
             auto e = value.as<VElement*>();
             if(e->focus) focal = key.as<int>();
             e->focus = false;
             count += 1;
        }
        
        bool prev = false;
        focal += (prev ? -1 : 1);
        if(focal > count) focal = 1;
        if(focal < 1) focal = count;
        lua["control"]["focus_idx"] = focal;

        for (const auto& key_value_pair : navigables ) {
             sol::object key = key_value_pair.first;
             sol::object value = key_value_pair.second;
             
             auto e = value.as<VElement*>();
             if(key.as<int>() == focal) {
                 e->focus = true;
                 break;
             }
        }

    };
    l_control["checkEvent"] = [=](int w, int ev, int note, int vel){

        if (ev == MidiBytes::on and vel == 0) {
            ev = MidiBytes::off;
        }

        sol::function action;
        if(w)
            action = lua["control"]["map"][ev][note];
        else 
            action = lua["control"]["unitmap"][ev][note];      
            
        if (action){
            lua["control"]["ev_vel"] = vel;
            action();
        }
        
        std::string s;
        s += "[";
        s += std::to_string(w);
        s += "] ev:";
        s += std::to_string(ev);
        s += " n:";
        s += std::to_string(note);
        s += " v:";
        s += std::to_string(vel);
        mevents.insert(mevents.begin(),s);
        if(mevents.size() > 5) mevents.pop_back();
    };
    
    //only supply a table of midi event mappings
    l_control["mapMidi"] = [=](sol::table mappings){
        
        mappings.for_each([=](sol::object const& key, sol::object const& value) {
            auto m = value.as<sol::table>();
            
            sol::table target = lua["control"]["map"];
            // sol::table target;
//             int d = m["device"];
//             if(d > 0) target = lua["control"]["map"];
//             else target = lua["control"]["unitmap"];
            
            std::string code = m["event_action"];
            auto lr = lua.load(code);
            if(lr.valid()){
                target[m["event"]][m["event_key"]] = lr.get<sol::protected_function>();
            }
        });
    };
    
    l_audio["que"].get_or_create<sol::table>();
    l_audio["que"]["clip"].get_or_create<sol::table>();
    l_audio["que"]["period"] = [](long p) {
        audioActionQue.period = p;
    };
    l_audio["que"]["tick"] = []() -> float {
        return audioActionQue.tick;
    };
    l_audio["que"]["progress"] = []() -> float {
        return audioActionQue.period_ratio;
    };
    l_audio["que"]["count"] = []() -> float {
        return audioActionQue.period_ticks;
    };
      
    l_audio["que"]["confirm"] = [](){
        audioActionQue.confirm();
    };
    l_audio["que"]["clear"] = [](){
        audioActionQue.clear();
    };
    
    l_audio["que"]["test"] = [](){
        audioActionQue.add(AudioActionQue::q_test,8,0.5f);
    };
    l_audio["que"]["rec"] = [](){
        audioActionQue.add(AudioActionQue::q_rec,0,0.f);
    };
    // l_audio["que"]["clip"]["clear"] = [](Clip* c, float r) -> int{
 //        return audioActionQue.add([=](){
 //            c->clear();
 //            c->update();
 //        },r);
 //    };
 //    l_audio["que"]["clip"]["stop"] = [](Clip* c, float r) -> int{
 //        return audioActionQue.add([=](){
 //            clip_stop(c);
 //            c->update();
 //        },r);
 //    };
 //    l_audio["que"]["clip"]["rec"] = [](Clip* c, float r) -> int{
 //        return audioActionQue.add([=](){
 //            clip_rec(c);
 //            c->update();
 //        },r);
 //    };
 //    l_audio["que"]["clip"]["play"] = [](Clip* c, float r) -> int{
 //        return audioActionQue.add([=](){
 //            clip_play(c);
 //            c->update();
 //        },r);
 //    };
    lua["test_clip"] = &test_clip;
    lua["test_clip2"] = &test_clip2;
    
	auto clip = lua.new_usertype<Clip>("Clip");
    l_audio["constants"].get_or_create<sol::table>();
    l_audio["constants"]["clip"].get_or_create<sol::table>();
    l_audio["constants"]["clip"]["states"].get_or_create<sol::table>();
    l_audio["constants"]["clip"]["states"]["clear"] = 0;
    l_audio["constants"]["clip"]["states"]["stop"]  = 1;
    l_audio["constants"]["clip"]["states"]["base"]  = 2;
    l_audio["constants"]["clip"]["states"]["play"]  = 3;
    l_audio["constants"]["clip"]["states"]["dub"]   = 4;
    l_audio["constants"]["clip"]["states"]["merge"]   = 5;
    clip["isClear"] = sol::property([](Clip* c){
        return (c->state == Clip::State::clear);
    });
    clip["isStopped"] = sol::property([](Clip* c){
        return (c->state == Clip::State::stop);
    });
    clip["isRecording"] = sol::property([](Clip* c){
        return (c->state == Clip::State::base or c->state == Clip::State::dub or c->state == Clip::State::merge);
    });
    clip["isDubbing"] = sol::property([](Clip* c){
        return (c->state == Clip::State::dub);
    });
    clip["isMerging"] = sol::property([](Clip* c){
        return (c->state == Clip::State::merge);
    });
    clip["isPlaying"] = sol::property([](Clip* c){
        return (c->state == Clip::State::play or c->state == Clip::State::dub);
    });
    clip.set("state", sol::readonly(&Clip::state));
    clip["length"] = sol::property([](Clip* c) -> unsigned long {return (unsigned long)c->length;});
    clip["head"] = sol::property([](Clip* c) -> float {
        if(c->length){
            return float(c->head) / float(c->length);
        }
        
        return 0.f;
    });
    // lua["clip_stop"] = [](Clip* c){clip_stop(c);};
 //    lua["clip_rec"] = [](Clip* c){clip_rec(c);};
 //    lua["clip_play"] = [](Clip* c){clip_play(c);};
 //
	auto elem = lua.new_usertype<VElement>("VElement");
    elem["id"] = sol::property([](VElement* v) -> unsigned long {return v->id;});
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
	elem["hue"] = sol::property([](VElement* v, float h){v->col = hueToHSV(h);});
	l_visuals["HUE_RED"] = 18;
	l_visuals["HUE_ORANGE"] = 26;
	l_visuals["HUE_YELLOW"] = 36;
	l_visuals["HUE_GREEN"] = 54;
	l_visuals["HUE_CYAN"] = 72;
	l_visuals["HUE_BLUE"] = 90;
	l_visuals["HUE_PURPLE"] = 108;
	l_visuals["HUE_WHITE"] = 127;
    
    elem["tag"] = &VElement::tag;
	elem["focus"] = sol::property([](VElement* v, bool a){
    	int i=1;
        for(auto e : elements){
    		e->focus = false;
            if(e == v) lua["focus_idx"] = i;
            i++;
    	} 
        v->focus = true;
    });
	
	auto lbl = lua.new_usertype<VLabel>("VLabel",sol::base_classes, sol::bases<VElement>());
	lbl["text"] = &VLabel::text;
	lbl["size"] = &VLabel::size;
	lbl["font"] = &VLabel::font;
	
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
	btn["selected"] = &VButton::selected;
	btn["action"] = sol::property([](VButton* b, std::function<void(void)> f){b->onPress = f;});
	btn["release"] = sol::property([](VButton* b, std::function<void(void)> f){b->onRelease = f;});
	btn["state"] = sol::property([](VButton* b)->bool{return b->state;}, [](VButton* b, bool s){ b->state = s; if(b->onPress and s) b->onPress(); });
	btn["onPress"] = [](VButton* b){b->onPress();};
	btn["onRelease"] = [](VButton* b){b->onRelease();};
    
	auto ubtn = lua.new_usertype<VUnitButton>("VUnitButton",sol::base_classes, sol::bases<VButton,VElement>());
	ubtn["note"] = &VUnitButton::note;
    ubtn["type"] = &VUnitButton::type;
    ubtn["v_hue"] = &VUnitButton::v_hue;
    
    
	l_visuals["createTexture"] = [](std::string t) -> Texture2D {
		auto tex = LoadTexture((root+t).c_str());
		textures_in_script.push_back(tex);
		return tex;
	};
	
	l_visuals["removeVElement"] = [](VElement* l){
        elements.erase(std::remove_if(
			elements.begin(), 
            elements.end(),
            [=](auto a){
            	if(a == l){
					return true;
            	}
            	return false;
            }),
            elements.end()
        );
	};
	l_visuals["addVElement"] = []() -> VElement* { 
		auto l = new VElement();
		elements.push_back(l);
		return l;
	};
	l_visuals["addVButton"] = [](sol::function f) -> VButton* { 
		auto l = new VButton();
		l->onPress = f;
		elements.push_back(l);
		return l;
	};
    l_visuals["addVUnitButton"] = [](int note) -> VUnitButton* { 
		auto l = new VUnitButton();
		l->note = note;
		elements.push_back(l);
		return l;
	};
	l_visuals["addVLabel"] = [](std::string s) -> VLabel* { 
		auto l = new VLabel();
		l->text = s;
		elements.push_back(l);
		return l;
	};
	l_visuals["addVImage"] = []() -> VImage* { 
		auto l = new VImage();
		elements.push_back(l);
		return l;
	};
	l_visuals["addVTimer"] = []() -> VTimer* { 
		auto l = new VTimer();
		elements.push_back(l);
		return l;
	};
	

	l_visuals["placeVElement"] = [](VElement* v, float x, float y){
		if(v == nullptr) {
			ERROR("placing invalid object");
			return;
		}
		v->x = x;
		v->y = y;
	};
	
	l_visuals["debugVElement"] = [](VElement* v){
		if(v == nullptr) {
			ERROR("invalid object");
			return;
		}
		
		v->bound_box = not v->bound_box;
	};
	l_visuals["moveVElement"] = [=](VElement* v, float x, float y, int in_time){
		if(v == nullptr) {
			ERROR("moving invalid object");
			return;
		}
        
        float tt = float(in_time*2);
        float sx = v->x;
        float sy = v->y;
        float dx = x-sx;
        float dy = y-sy;
        
        sol::table ac = lua.create_table_with("duration",in_time,"action",
            [=](float dt){
                v->x = sx + (dt/tt)*dx;
                v->y = sy + (dt/tt)*dy;
            }
        );
		lua["visuals"]["addVAction"](ac,sol::optional<std::string>("move_linear"));
	};	
	l_visuals["slideVElement"] = [=](VElement* v, float x, float y, int in_time){
		if(v == nullptr) {
			ERROR("moving invalid object");
			return;
		}
        
        float tt = float(in_time*2);
        float sx = v->x;
        float sy = v->y;
        float dx = x-sx;
        float dy = y-sy;
        
        sol::table ac = lua.create_table_with("duration",in_time,"action",
            [=](float dt){
                v->x = sx + std::pow(std::sin(3.1415f * dt/tt),2)*dx;
                v->y = sy + std::pow(std::sin(3.1415f * dt/tt),2)*dy;
            }
        );
		lua["visuals"]["addVAction"](ac,sol::optional<std::string>("move_smooth"));
	};
	l_visuals["offsetVElement"] = [](VElement* v, float x, float y){
		if(v == nullptr) {
			ERROR("offsetting invalid object");
			return;
		}
		v->x += x;
		v->y += y;
	};
    
    l_visuals["cancelVSequence"] = [](std::string withName){
        actions.erase(std::remove_if(
            actions.begin(),
                actions.end(),
                [=](auto s){
                    if(s->name == withName){
                    return true;
                    }
                    return false;
                }),
                actions.end()
            );
    };
	
	l_visuals["addVAction"] = [](sol::table ac,sol::optional<std::string> name){
		
        /*
        VAction table:
        ["action"]
        ["duration"]
        */
        
        /*
        AddVAction({
            duration = 1000, <- ms
            action = function()
                print("test action")
            end
        },"action key")
        */
		auto a = new VAction();
        a->time_to_take = std::chrono::milliseconds{ac["duration"]};
		a->action = ac["action"];
        
        auto s = new VSequence();
		s->time_current = std::chrono::milliseconds{0};
	    s->actions.push_back(a);
        s->exec_ratio = 0.0f;
        s->count = 1;
        
        if(name == sol::nullopt) s->name = "default";
        else s->name = name.value();
        LOG("new action: " + s->name);
		actions.push_back(s);
	};
    l_visuals["addVSequence"] = [](sol::table ac,sol::optional<std::string> name){
        /*
        AddVSequnece(
        {
            {
                duration = 1000, <- ms
                action = function()
                    print("first action")
                end
            },
            {
                duration = 2000, <- ms
                action = function()  
                    print("next action")
                end
            }
        },"actions key")
        */
        
        auto s = new VSequence();
		s->time_current = std::chrono::milliseconds{0};
        s->exec_ratio = 0.0f;
        s->count = 0;
	    
        if(name == sol::nullopt) s->name = "default";
        else s->name = name.value();
        LOG(s->name);

        ac.for_each([=](sol::object const& key, sol::object const& value) {
            auto t = value.as<sol::table>();
    		auto a = new VAction();
            a->time_to_take = std::chrono::milliseconds{t["duration"]};
    		a->action = t["action"];
            s->actions.push_back(a);
            s->count += 1;
        });
		actions.push_back(s);
    };
    
	l_visuals["message"] = [](std::string msg){
		message_text = msg;
		message_timer = std::chrono::duration_cast<fpstime>(std::chrono::seconds{2});
	};
	l_visuals["setHue"] = [=](int i, int hue){
	    lua["control"]["units"][i]["v_hue"] = hue;
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

	lua.script_file(root + "scripts/_init.lua");
};

void script(){
	onFrame = nullptr;
	onUIFrame = nullptr;
	for(auto e : elements) delete e;
	for(auto a : actions) delete a;
	elements.clear();
	actions.clear();
	for(auto t: textures_in_script){
		UnloadTexture(t);
	}
	textures_in_script.clear();
    lua["control"]["navigables"] = nullptr;
	lua["visuals"]["onUIReload"]();	
    
	//load screen script
	try{		
		auto sc = lua.load_file(root + chain.back());
		auto result = sc();
		script_error = not result.valid();
		if (result.valid()) {
            onUIFrame = lua["visuals"]["onUIFrame"];
			onFrame = lua["visuals"]["onFrame"];
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
;

void check_script()
{
	if((IsKeyPressed(KEY_R) and IS_SHIFT_DOWN and not in_console) or reload){
		script();
		reload = false;
	}		
};


void do_actions()
{ 
    using namespace std::chrono;

	auto timer_actions = new ScopedTimer(&bench_actions);
	if(actions_view) DrawString("Actions:",0,0,16,GRAY);
	int yy = 1;
	for (auto seq : actions){
        if(seq->actions.size() == 0) continue;
        
        auto ac = seq->actions.front();
        if(ac->action)
        ac->action((seq->time_current).count());
		seq->time_current += time_ac_dur;
		seq->exec_ratio = float(seq->time_current.count()) / float(ac->time_to_take.count());
        
        if(actions_view) {
            auto bounds = DrawString("("
            + std::to_string((ac->time_to_take).count())
            + "ms):" 
            + seq->name
            + ":"
            + std::to_string(seq->count - seq->actions.size() + 1)
            + "/" + std::to_string(seq->count)
                , 0, 16*yy, 16, WHITE);
            DrawRectangle(0,16*yy++, bounds.x*seq->exec_ratio, 16,{0,255,0,64});
        }
	 
		//DrawString("[" + std::to_string(int(a->exec_ratio*100)) + "%] -> " + ac->name, 0, 16*yy++, 16, GRAY);
    	
        if(seq->time_current >= ac->time_to_take and seq->actions.size()){
            seq->actions.erase(seq->actions.begin());
            seq->time_current = std::chrono::milliseconds{0};
        }    
	}
	actions.erase(std::remove_if(
		actions.begin(), 
		actions.end(),
		[](auto a){
			if(a->actions.size() == 0){
				return true;
			}
			return false;
		}),
		actions.end()
	);
	delete timer_actions;
};

void do_elements()
{
	auto timer_elem = new ScopedTimer(&bench_elements);
	for(auto e : elements){
		e->update();
	}
	delete timer_elem;
}

void do_grid()
{
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
		if(m.y > S_HEIGHT) return;
		
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

void do_console()
{
	
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

	DrawRectangle(16,S_HEIGHT,S_WIDTH-32,16,(in_console ? DARKGRAY : (IS_SHIFT_DOWN ? RED : BLACK)));
	DrawString("->" + command,16,S_HEIGHT);
	if(not in_console)DrawRectangleLines(16,S_HEIGHT,S_WIDTH-32,16,DARKGRAY);	
	
	if(IsKeyPressed(KEY_GRAVE)) in_console = not in_console;

}

void screen()
{
    time_ac_old = time_ac_now = std::chrono::high_resolution_clock::now();
	while(not WindowShouldClose()){
         
        time_ac_old = time_ac_now;
        time_ac_now = std::chrono::high_resolution_clock::now();
        time_ac_dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_ac_now-time_ac_old);

        BeginDrawing();
        ClearBackground(BLACK);
        auto timer_fps = new ScopedTimer(&bench_fps);

        {
            std::lock_guard<std::mutex> lg(mtx_fps);
            auto timer_frame = new ScopedTimer(&bench_frame);
            if(onFrame) onFrame();
            if(onUIFrame) onUIFrame();
        }

        {
        std::lock_guard<std::mutex> lg(mtx_fps);
        do_elements();
        do_actions();
        }

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
            float ratio_dsp = float(bench_dsp.count()) / float(max_dsp.count());
            float ratio_total = float(total.count());

            DrawRectangleLines(524,8,100.0f,8,GREEN);
            DrawRectangle(524,8,100.0f*ratio_fps,8,GREEN);

            DrawRectangleLines(524,16,100.0f,8,WHITE);
            DrawRectangle(524,16,100.0f*ratio_frame,8,PURPLE);
            DrawRectangle(524+100.0f*ratio_frame,16,100.0f*ratio_actions,8,YELLOW);
            DrawRectangle(524+100.0f*ratio_actions,16,100.0f*ratio_elements,8,SKYBLUE);
            DrawString("[Fps] " + std::to_string(100.0f*ratio_fps)+"%",500,32,16,GREEN);
            DrawString("[Frm] " + std::to_string(100.0f*ratio_frame)+"%",500,48,16,PURPLE);
            DrawString("[Act] " + std::to_string(100.0f*ratio_actions)+"%",500,64,16,YELLOW);
            DrawString("[Elm] " + std::to_string(100.0f*ratio_elements)+"%",500,80,16,SKYBLUE);

            DrawRectangleLines(524,24,100.0f,8,WHITE);
            DrawRectangle(524,24,100.0f*ratio_dsp,8,RED);
            DrawString("[DSP] " + std::to_string(100.0f*ratio_dsp)+"%",500,96,16,RED);

        }

        if(chain_view){
            int y = 0;
            int x = 0;
            for(auto s : chain){
                if(x==chain.size()-1) s += "<";
                DrawString(s,16*x++,S_HEIGHT-16*y++,16,WHITE,0.0,1.0);
            }
        }
        if(midi_view){
            int y = 0;
            int x = 0;
            for(auto s : mevents){
                DrawString(s,S_WIDTH-16,S_HEIGHT-16*y,16,{255,255,255,127+(127*(5-y)/5)},1.0,1.0);
                y++;
            }
        }

        do_console();

        EndDrawing();

        {
        std::lock_guard<std::mutex> lg(mtx_fps);
        check_script();
        }
	}
	CloseWindow();
}


void pollCtrl()
{
    std::lock_guard<std::mutex> lg(mtx_fps);
	Vector2 m = GetMousePosition();
	bool bb = IsMouseButtonDown(0);
	static bool bo;
	for(auto e : elements)
	{
		if(VUnitButton* b = dynamic_cast<VUnitButton*>(e))
		{	
			float xx = b->x - b->w*b->a_x;
			float yy = b->y - b->h*b->a_y;
			auto r = Rectangle{xx,yy,b->w,b->h};
			if(CheckCollisionPointRec(m,r)){
                if((not b->state and bb and not bo) or (b->state and not bb and bo)){
					b->state = bb;
                    int vel = b->state*127;
                    lua["control"]["checkEvent"](0,b->type,b->note,vel);
                }
			}
            
            if(b->v_hue != b->v_hue_old){
                b->v_hue_old = b->v_hue; 
                //send midi to physical hue controller here
                b->col = hueToHSV(b->v_hue);
            }
		}
        else if(VButton* b = dynamic_cast<VButton*>(e))
		{	
			float xx = b->x - b->w*b->a_x;
			float yy = b->y - b->h*b->a_y;
			auto r = Rectangle{xx,yy,b->w,b->h};
			if(CheckCollisionPointRec(m,r)){				
                if(not b->state and bb and not bo)
                {
				    b->state = bb;
                    if(b->onPress) b->onPress();
				}
                else if(b->state and not bb and bo){
				    b->state = bb;
                    if(b->onRelease) b->onRelease();
                }
			}
		}
	}
	bo = bb;
}

void pollMidi()
{
	//connection management
	unsigned int nDevicesCount = scanner.getPortCount();
	if(nDevicesCount != devicesCount){
		LOG((nDevicesCount > devicesCount ? "Midi connect" : "Midi disconnect"));
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

			for (int i=0; i<nBytes; i++ ){
				bytes[n] = rawmidi[i];
				n++;
				if(n==3){
					n = 0;
					//LOG(m.print());
                    std::lock_guard<std::mutex> lg(mtx_fps);
                    lua["control"]["checkEvent"](d,bytes[0],bytes[1],bytes[2]);
					//checkEvent(&m);
				}
			}
		}
	}
}

void init()
{
	current_script = "scripts/screen_root.lua";
	chain.push_back(current_script);
	
	InitWindow(S_WIDTH,S_HEIGHT_T,"M O T I F (luatif)");
	SetTargetFPS(30);
	commands.push_back("");
	command = "";
	
	lua_init();
	script();

    LOG("audio init status:");
    LOG(audio_init());
}


int main(int argc, char* argv[]) 
{
	
    makeRoot(argv[0]);

    init();
    static std::atomic_bool running = true;
    std::thread thread_input([=](){
        while(running){
            pollMidi();
            pollCtrl();
            usleep(10000);
        }
    });
    std::thread thread_audio_supervise([=](){
        while(running){
            audio_supervisor();
            usleep(10000);
        }
    });

    screen();
    
    running = false;
    thread_input.join();
    thread_audio_supervise.join();
    audio_end();
	//////////////////
    // audio_init();
 //    while(1);
    
	return 0;
}

