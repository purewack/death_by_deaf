title = visuals.addVLabel("Screen 3!")
title.x = 320
title.y = 240

t = 0
visuals.onFrame = function()
    local x = 320 + (50 * math.sin(t) );
    title.x = x
    t = t +0.06
end

btn = visuals.ddVButton(function ()
	Dismiss()
end)
btn.w = 40
btn.h = 40

print("script 3")