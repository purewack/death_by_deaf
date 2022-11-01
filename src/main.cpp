#include "deaf.hpp"

void init();
void pollCtrl();
void screen();
void script();

int main(int argc, char* argv[]) 
{
	LOG(VER);
    init();
    audio_init();

    static std::atomic_bool running = true;
    std::thread thread_input([=](){
        while(running){
            pollCtrl();
            usleep(16666);
        }
    });

    screen();
    
    running = false;
    thread_input.join();
    audio_end();

	return 0;
}


void init()
{
	current_script = "scripts/screen_root.lua";
	chain.push_back(current_script);
	commands.push_back("");
	command = "";
	
	InitWindow(S_WIDTH,S_HEIGHT_T,"DEAF Engine");
	SetTargetFPS(
		#ifdef WIN32
		120
		#else
		60
		#endif
	);

	lua_init();
	script();
	
	puppet.cam = { { 0.0f, 1.5f, 0.0f }, { 0.0f, 1.5f, 1.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
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


void script(){
	onFrame = nullptr;
	onUIFrame = nullptr;
	for(auto e : elements) delete e;
	for(auto a : actions) delete a;
	elements.clear();
	actions.clear();
	for(auto t: textures_in_script){
		UnloadTexture(t.texture);
	}
	for(auto m: models_in_script){
		UnloadModel(m.mesh);
	}
	textures_in_script.clear();
	models_in_script.clear();
    lua["control"]["navigables"] = nullptr;
	lua["visuals"]["onUIReload"]();	
    
	//load screen script
	try{		
		auto sc = lua.load_file(chain.back());
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
            + STR((ac->time_to_take).count())
            + "ms):" 
            + seq->name
            + ":"
            + STR(seq->count - seq->actions.size() + 1)
            + "/" + STR(seq->count)
                , 0, 16*yy, 16, WHITE);
            DrawRectangle(0,16*yy++, bounds.x*seq->exec_ratio, 16,{0,255,0,64});
        }
	 
		//DrawString("[" + STR(int(a->exec_ratio*100)) + "%] -> " + ac->name, 0, 16*yy++, 16, GRAY);
    	
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

void do_objects()
{
    BeginMode3D(puppet.cam);
        if(floor_view){
            DrawCube({0.5f,0.f,0.f}, 1.0f,0.1f,0.1f,RED);
            DrawCube({0.f,0.5f,0.f}, 0.1f,1.0f,0.1f,GREEN);
            DrawCube({0.f,0.f,0.5f}, 0.1f,0.1f,1.0f,BLUE);
            DrawGrid(40, 1.0f);        // Draw a grid
        }
        for(auto o : objects){
            o->draw();
        }
    EndMode3D();
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
		coords += STR(gx);
		coords += ",";
		coords += STR(gy);
		coords += '\n';
		coords += STR(m.x);
		coords += ",";
		coords += STR(m.y);
	
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
	//SetTraceLogLevel(LOG_ERROR);
    std::cout << "[gfx]" << std::endl;
    time_ac_old = time_ac_now = std::chrono::high_resolution_clock::now();
	while(not WindowShouldClose()){
         
        time_ac_old = time_ac_now;
        time_ac_now = std::chrono::high_resolution_clock::now();
        time_ac_dur = std::chrono::duration_cast<std::chrono::milliseconds>(time_ac_now-time_ac_old);

        BeginDrawing();
        ClearBackground(BLACK);
        auto timer_fps = new ScopedTimer(&bench_fps);

		//onSceneVideoFrame();

        {
            std::lock_guard<std::mutex> lg(mtx_fps);
            auto timer_frame = new ScopedTimer(&bench_frame);
            if(onFrame) onFrame();
            if(onUIFrame) onUIFrame();
        }

        {
        std::lock_guard<std::mutex> lg(mtx_fps);
        do_objects();
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
            DrawString("[Fps] " + STR(100.0f*ratio_fps)+"%",500,32,16,GREEN);
            DrawString("[Frm] " + STR(100.0f*ratio_frame)+"%",500,48,16,PURPLE);
            DrawString("[Act] " + STR(100.0f*ratio_actions)+"%",500,64,16,YELLOW);
            DrawString("[Elm] " + STR(100.0f*ratio_elements)+"%",500,80,16,SKYBLUE);

            DrawRectangleLines(524,24,100.0f,8,WHITE);
            DrawRectangle(524,24,100.0f*ratio_dsp,8,RED);
            DrawString("[DSP] " + STR(100.0f*ratio_dsp)+"%",500,96,16,RED);

        }

		// if(audio_que_view){
		// 	DrawString("AQ [" + STR(audioActionQue.actions_count) + "] <- [" + STR(audioActionQue.actions_count) + "]", 0,S_HEIGHT-4,16,WHITE,0.0,1.0);
		// 	for(int i=0; i<audioActionQue.max_actions; i++){
		// 		std::string s = STR(i) + " > ";
		// 		if(audioActionQue.actions[i].pending)
		// 			s += audioActionQue.actions[i].name + "@" + STR(audioActionQue.actions[i].offset);
		// 		else s += " -- ";
		// 		if(audioActionQue.que[i].pending)
		// 			s += audioActionQue.que[i].name + "@" + STR(audioActionQue.que[i].offset);
		// 		else s += "| -- ";
				
		// 		DrawString(s,0,S_HEIGHT-16 - 8*i, 16,WHITE,0.0,1.0);
		// 	}
		// }
        // else 
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

        DrawFPS(700, 10);

        EndDrawing();

        {
        std::lock_guard<std::mutex> lg(mtx_fps);
        check_script();
        }
	}
	CloseWindow();
}

float dt = 0.f;
bool paused = false;
void pollCtrl()
{
	// if(audio_editor)
	// 	libpd_poll_gui();

	auto m = GetMousePosition();
	puppet.mpos_new = m;
    std::lock_guard<std::mutex> lg(mtx_fps);
	bool bb = IsMouseButtonDown(0);
	static bool bo;
	for(auto e : elements)
	{
		if(VButton* b = dynamic_cast<VButton*>(e))
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

	if(not in_console)
	lua_system["onLogicLoop"]();
}
