#include "deaf.hpp"

void VElement::drawBoundBox(){
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
        pos += STR(x);
        pos += ",";
        pos += STR(y);
        DrawString(pos,x,y);
    }
    if(focus){
        DrawRectangle(xx,yy,w,h,{0,255,255,128});
    }
}
	
void VElement::update(){
    if(visible and col.a) draw();
    drawBoundBox();
}

void VElement::draw(){
    if(w == 0 or h == 0) return;

    float xx = x - w*a_x;
    float yy = y - h*a_y;
    
    DrawRectangle(xx,yy, w, h, col);
};
void VElement_bind(){
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
    lua_visuals["addVElement"] = []() -> VElement* { 
		auto l = new VElement();
		elements.push_back(l);
		return l;
	};
}



void VLabel::draw() {
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
void VLabel_bind(){
    auto lbl = lua.new_usertype<VLabel>("VLabel",sol::base_classes, sol::bases<VElement>());
	lbl["text"] = &VLabel::text;
	lbl["size"] = &VLabel::size;
	lbl["font"] = &VLabel::font;
    lua_visuals["addVLabel"] = [](std::string s) -> VLabel* { 
		auto l = new VLabel();
		l->text = s;
		elements.push_back(l);
		return l;
	};
}



void VButton::draw()  {	
    float xx = x - w*a_x;
    float yy = y - h*a_y;	
    Rectangle r = {xx,yy,w,h};
    DrawRectangleLines(xx,yy,w,h,WHITE);
    DrawRectangle(xx+2,yy+2,w-4,h-4,(state ? WHITE : col));
    if(selected) DrawRectangleLines(xx-2,yy-2,w+4,h+4,WHITE);
}
void VButton_bind(){
    auto btn = lua.new_usertype<VButton>("VButton",sol::base_classes, sol::bases<VElement>());
	btn["selected"] = &VButton::selected;
	btn["action"] = sol::property([](VButton* b, std::function<void(void)> f){b->onPress = f;});
	btn["release"] = sol::property([](VButton* b, std::function<void(void)> f){b->onRelease = f;});
	btn["state"] = sol::property([](VButton* b)->bool{return b->state;}, [](VButton* b, bool s){ b->state = s; if(b->onPress and s) b->onPress(); });
	btn["onPress"] = [](VButton* b){b->onPress();};
	btn["onRelease"] = [](VButton* b){b->onRelease();};
    lua_visuals["addVButton"] = [](sol::function f) -> VButton* { 
		auto l = new VButton();
		l->onPress = f;
		elements.push_back(l);
		return l;
	};
}	


void VImage::SetTexture(Texture2D t){
    tex = t;
    w = tex.width;
    h = tex.height;
    tmx = 1;
    tmy = 1;
    tx = 0;
    ty = 0;
}

void VImage::draw() {
    //VElement::draw();
    if(tex.id){
        float sw = tex.width/tmx;
        float sh = tex.height/tmy;
        float sx = sw*tx;
        float sy = sw*ty;
    
        DrawTexturePro(tex,
            Rectangle{sx,sy,sw,sh},
            Rectangle{x - w*a_x, y - h*a_y, w, h},
            Vector2{0,0},0,WHITE);
    }
};
void VImage_bind(){
    lua.new_usertype<Texture2D>("VTexture");
	auto image = lua.new_usertype<VImage>("VImage",sol::base_classes, sol::bases<VElement>());
	image["tex"] = sol::property(&VImage::SetTexture);
	image["tiles_count_x"] = &VImage::tmx;
	image["tiles_count_y"] = &VImage::tmy;
	image["tile_x"] = &VImage::tx;
	image["tile_y"] = &VImage::ty;
    lua_visuals["addVImage"] = []() -> VImage* { 
		auto l = new VImage();
		elements.push_back(l);
		return l;
	};
}



VObject::VObject(std::string path){
    model = LoadModel(path.c_str());
}
VObject::~VObject(){
    UnloadModel(model);
}
void VObject::draw(){
    DrawModelEx(model,{x,y,z},{ax_x,ax_y,ax_z},ax_angle,{scale,scale,scale},col);
}
void VObject_bind(){
	auto obj = lua.new_usertype<VObject>("VObject",sol::base_classes, sol::bases<VElement>());
    obj["z"] = &VObject::z;
    obj["az"] = &VObject::a_z;
    obj["axis_x"] = &VObject::ax_x;
    obj["axis_z"] = &VObject::ax_z;
    obj["axis_y"] = &VObject::ax_y;
    obj["axis_angle"] = &VObject::ax_angle;
    obj["scale"] = &VObject::scale;
    lua_visuals["addVObject"] = [](std::string model_path) -> VObject* {
        auto m = new VObject(model_path);
        objects.push_back(m);
        return m;
    };
	
}




void lua_Vbind(){
    VElement_bind();
    VButton_bind();
    VLabel_bind();
    VImage_bind();
    VObject_bind();
}

