--Actions(true)
--Bench(true)
--Chain(true)
--Grid(true)

clips = {}

for x=0,3 do
	clips[x] = AddVTimer()
	clips[x].w = 50
	clips[x].h = 50
	GridVElement(clips[x],2.5+x,1.5)
end
clips[0].hue = 0

tex1 = CreateTexture("test_tex.png")
img = AddVImage();
img.x = 300
img.y = 200
img.tex = tex1
img.w = 100
img.h = 100


tiles = CreateTexture("autotile_tileset.png")
img2 = AddVImage();
img2.x = 400
img2.y = 200
img2.tex = tiles
img2.w = 90
img2.h = 90
img2.tiles_count_x = 9
img2.tiles_count_y = 6
img2.tile_x = 0
img2.tile_y = 0

btn = AddVButton(function ()
	clips[0].progress = 0

	AddVAction({
		name="timer",
		duration=2000,
		action = function (dt)
			clips[0].progress = dt/2000
		end,
		completion = function ()
			clips[0].progress = 0
		end,
		singleton = true
	})
		
end);
btn.w = 54
btn.h = 54
btn.a_x = 0.0
btn.a_y = 0.0

nxt = AddVButton(function()
	Present("scripts/screen2.lua")
end)
nxt.w = 40
nxt.h = 40
nxt.x = 600
nxt.y = 400


nxt2 = AddVButton(function()
	Message("Skipped screen")
	Present("scripts/screen3.lua")
end)
nxt2.w = 40
nxt2.h = 40
nxt2.x = 500
nxt2.y = 400

lbl = AddVLabel("hello");
lbl.a_x = 0
lbl.a_y = 0
lbl.font_idx = 1

t = 0
onFrame = function ()
	local x = 320 + (200 * math.sin(t) );
    local y = 288 - (100 * math.sin(t*2) );
    lbl.x = x;
  	lbl.y = y;
  	t = t + 0.02;
end

onLoad = function ()
	xs = sin(0.4)
	print("loaded now")
	print(xs)
end