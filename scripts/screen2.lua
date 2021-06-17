title = AddVLabel("Screen 2!")
title.x = 320
title.y = 240

t = 0
onFrame = function()
	local y = 240 + (50 * math.sin(t) );
    title.y = y
    t = t +0.06
end

btn = AddVButton(function ()
	Dismiss()
end)
btn.w = 40
btn.h = 40

nxt = AddVButton(function()
	Present("scripts/screen3.lua")
end)
nxt.w = 40
nxt.h = 40
nxt.x = 600
nxt.y = 400
