#include "deaf.hpp"

Vector2 DrawString(std::string str ,float x, float y, float s, Color c, float anchorX, float anchorY, Font f)
{
	Vector2 bounds = MeasureTextEx(f,str.c_str(),s,0);
	Vector2 pos = {x - bounds.x*anchorX, y- bounds.y*anchorY};
	DrawTextEx(f,str.c_str(), pos, s, 0, c);
	return bounds;
}

VSequence::~VSequence(){
    for(auto a : actions) delete a;
    actions.clear();
}

int lua_log_ex(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
	
    std::cout << "Script: An exception occurred in a function ";
	if (maybe_exception) {
		const std::exception& ex = *maybe_exception;
		printLog(ex.what());
	}

	return sol::stack::push(L, description);
}

void lua_init(){
    lua.set_exception_handler(lua_log_ex);

	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::io);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::os);
	lua["S_W"] = S_WIDTH;
	lua["S_H"] = S_HEIGHT;
	lua.require_file("json","scripts/json.lua");
	
    lua_system = lua["system"].get_or_create<sol::table>();
    lua_visuals = lua["visuals"].get_or_create<sol::table>();
    lua_control = lua["control"].get_or_create<sol::table>();
    lua_audio = lua["audio"].get_or_create<sol::table>();

	lua_system["loadFont"] = [](std::string name){
    	auto f = LoadFont((name).c_str());
        fonts.push_back(f);
	};
	lua_system["Present"] = [](std::string scr){
		current_script = scr;
		chain.push_back(scr);
		reload = true;
	};
	lua_system["Dismiss"] = [](){
		if(chain.size() > 1) chain.pop_back();
		current_script = chain.back();
		reload = true;
	};
    lua_visuals["uiFont"] = [](int i){
        if(i >= 0 and i < fonts.size()) ui_font = fonts[i];
    };
	lua_visuals["gridDiv"] = [](int d){
		g_div = d;
	};
	lua_visuals["gridW"] = []() -> int{
		return (S_WIDTH)/g_div;
	};
	lua_visuals["gridH"] = []() -> int{
		return (S_HEIGHT)/g_div;
	};
	lua_visuals["grid"] = [](bool state){
		layout_grid = state;
	};
	lua_visuals["actions"] = [](bool state){
		actions_view = state;
	};
	lua_visuals["bench"] = [](bool state){
		bench_view = state;
	};
	lua_visuals["chain"] = [](bool state){
		chain_view = state;
	};
    lua_visuals["floor"] = [](bool state){
		floor_view = state;
	};
	lua_system["toggleAnimation"] = [](){
		if(onFrame) onFrame = nullptr;
		else onFrame = lua["onFrame"];
	};
    lua_system["push_command"] = [](std::string cmd){
        commands.push_back(cmd);
    };
    lua_system["async_after"] = [=](sol::function action, int after_ms, std::string withName){  
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
    lua_system["async_cancel"] = [=](std::string withName){
        lua_visuals["cancelVSequence"](withName);
    };
    
    lua_system["readJSON"] = [=](std::string path){
        lua["io"]["input"](path,"r");
        auto text = lua["io"]["read"]("*all");
        lua["current_json"] = lua["json"]["decode"](text);
        lua.script("io.input():close()");
    };
    lua_system["writeJSON"] = [=](std::string path){        
        lua["io"]["output"](path,"w");
        auto to_write = lua["json"]["encode"](lua["current_json"]);
        lua["io"]["write"](to_write);
        lua.script("io.output():close()");
    };

    lua_control["isKeyDown"] = [=](int key) -> bool {
        return IsKeyDown(key);
    };
    lua_control["isKeyUp"] = [=](int key) -> bool {
        return IsKeyUp(key);
    };
    lua_control["isKeyPressed"] = [=](int key) -> bool {
        return IsKeyPressed(key);
    }; 
    lua_control["isKeyReleased"] = [=](int key) -> bool {
        return IsKeyReleased(key);
    };
    lua_control["getKeyPressed"] = [=]() -> int {
        return GetKeyPressed();
    };
    lua_control["focus_idx"] = (int)0;
    lua_control["navigables"].get_or_create<sol::table>();
    lua_control["navigate"] = [=](){
        sol::table navigables = lua_control["navigables"];
        if(navigables == sol::lua_nil) return;
        
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

	
    lua_visuals["removeVObject"] = [](VObject* l){
        objects.erase(std::remove_if(
			objects.begin(), 
            objects.end(),
            [=](auto a){
            	if(a == l){
					return true;
            	}
            	return false;
            }),
            objects.end()
        );
	};
	lua_visuals["removeVElement"] = [](VElement* l){
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
	

	lua_visuals["placeVElement"] = [](VElement* v, float x, float y){
		if(v == nullptr) {
			ERROR("placing invalid object");
			return;
		}
		v->x = x;
		v->y = y;
	};
	
	lua_visuals["debugVElement"] = [](VElement* v){
		if(v == nullptr) {
			ERROR("invalid object");
			return;
		}
		
		v->bound_box = not v->bound_box;
	};
	lua_visuals["moveVElement"] = [=](VElement* v, float x, float y, int in_time){
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
	lua_visuals["slideVElement"] = [=](VElement* v, float x, float y, int in_time){
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
	lua_visuals["offsetVElement"] = [](VElement* v, float x, float y){
		if(v == nullptr) {
			ERROR("offsetting invalid object");
			return;
		}
		v->x += x;
		v->y += y;
	};
    
    lua_visuals["cancelVSequence"] = [](std::string withName){
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
	
	lua_visuals["addVAction"] = [](sol::table ac,sol::optional<std::string> name){
		
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
    lua_visuals["addVSequence"] = [](sol::table ac,sol::optional<std::string> name){
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
    
	lua_visuals["message"] = [](std::string msg){
		message_text = msg;
		message_timer = std::chrono::duration_cast<fpstime>(std::chrono::seconds{2});
	};

    lua_visuals["mouse"] = [](bool state){
        if(state)
            EnableCursor();
        else
            DisableCursor();
	};    
    lua_visuals["fullscreen"] = [](bool state){
        if(IsWindowFullscreen() && state) return;
        if(!IsWindowFullscreen() && !state) return;
        ToggleFullscreen();
	};

    lua_Vbind();

	lua.script_file("scripts/_init.lua");
}