
print(GridW())
print(GridH())


tex1 = CreateTexture("test_tex.png")
img = AddVImage();
img.x = 300
img.y = 200
img.tex = tex1
img.w = 100
img.h = 100


tiles = CreateTexture("test_tiles.png")
img2 = AddVImage();
img2.x = 400
img2.y = 200
img2.tex = tiles
img2.w = 90
img2.h = 90
img2.tiles_count_x = 9
img2.tiles_count_y = 6
img2.tile_x = 1
img2.tile_y = 0


nxt = AddVButton(function()
Present("scripts/screen2.lua")
end)
nxt.w = 40
nxt.h = 40
nxt.x = 600
nxt.y = 400

nxt2 = AddVButton(function()
Present("scripts/screen3.lua")
Message("Skipped seq.")
end)
nxt2.w = 40
nxt2.h = 40
nxt2.x = 500
nxt2.y = 400

sv = AddVButton(function()
    
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
    AddVSequence({first,waitt,second},"lol")
end)
sv.gx = 1
sv.gy = 7
sv.tag = "sv"

nx = AddVButton(function ()
onNavigation(true)
end)
nx.gx = 2
nx.gy = 7

lbl = AddVLabel("hello");
lbl.font = 1
lbl.size = 40

t = 0
onFrame = function ()
local x = 320 + (200 * math.sin(t) );
   	local y = 288 - (100 * math.sin(t*2) );
   	lbl.x = x;
 	lbl.y = y;
 	t = t + 0.02;
end

