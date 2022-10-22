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



VImage::~VImage() {
    UnloadTexture(tex);
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


void VButton::draw()  {	
    float xx = x - w*a_x;
    float yy = y - h*a_y;	
    Rectangle r = {xx,yy,w,h};
    DrawRectangleLines(xx,yy,w,h,WHITE);
    DrawRectangle(xx+2,yy+2,w-4,h-4,(state ? WHITE : col));
    if(selected) DrawRectangleLines(xx-2,yy-2,w+4,h+4,WHITE);
}	
//void onPress();


void VTimer::draw() {
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
