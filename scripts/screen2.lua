title = visuals.addVLabel("Screen 2!")
title.x = 320
title.y = 240

t = 0
visuals.onFrame = function()
	local y = 240 + (50 * math.sin(t) );
    title.y = y
    t = t +0.06
end

btn = visuals.addVButton(function ()
	Dismiss()
end)
btn.w = 40
btn.h = 40

nxt = visuals.addVButton(function()
	Present("scripts/screen3.lua")
end)
nxt.w = 40
nxt.h = 40
nxt.x = 600
nxt.y = 400

unitmap[ev_note_on][key_pad2] = function()
   print("post added event screen 2") 
end