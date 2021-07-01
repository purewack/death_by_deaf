tex1 = visuals.createTexture("gfx/test_tex.png")
img = visuals.addVImage();
img.x = 300
img.y = 200
img.tex = tex1
img.w = 100
img.h = 100


tiles = visuals.createTexture("gfx/test_tiles.png")
img2 = visuals.addVImage();
img2.x = 400
img2.y = 200
img2.tex = tiles
img2.w = 90
img2.h = 90
img2.tiles_count_x = 9
img2.tiles_count_y = 6
img2.tile_x = 1
img2.tile_y = 0


nxt = visuals.addVButton(function()
Present("scripts/screen2.lua")
end)
nxt.w = 40
nxt.h = 40
nxt.x = 600
nxt.y = 400

nxt2 = visuals.addVButton(function()
Present("scripts/screen3.lua")
visuals.message("Skipped seq.")
end)
nxt2.w = 40
nxt2.h = 40
nxt2.x = 500
nxt2.y = 400

sv = visuals.addVButton(function()
    
    first = {
        duration = 2000,
        action = function (dt)
            if dt == 0 then
                print("first")
            end
        end
    };
    waitt = {
        duration = 1000
    }
    second = {
        duration = 3000,
        action = function (dt)
            print("second " .. dt)
        end
    }
    --CancelVAction("lol")
    visuals.addVSequence({first,waitt,second},"lol")
end)
sv.gx = 1
sv.gy = 7
sv.tag = "sv"



lbl = visuals.addVLabel("hello");
lbl.font = 1
lbl.size = 40

t = 0
visuals.onFrame = function ()
local x = 320 + (200 * math.sin(t) );
   	local y = 288 - (100 * math.sin(t*2) );
   	lbl.x = x;
 	lbl.y = y;
 	t = t + 0.02;
end

control.navigables = {lbl,sv}

control.unitmap[control.ev_note_on][control.key_pad2] = function()
   print("post added event")
end
