controls = {}
local c_names = {}
key_shift = 1
key_stop = 2
key_rec = 3
key_play = 4
key_pad1 = 5
key_less = 21
key_ok = 22
key_more = 23

loadfile(root .. "scripts/midi.lua")()

onUIReload = function () 
	
	c_names[key_shift] = "key_shift"
	c_names[key_stop] = "key_stop"
	c_names[key_rec] = "key_rec"
	c_names[key_play] = "key_play"

	for p=1,16 do
	c_names[key_pad1+p-1] = "key_pad_" .. p
	end
	
	c_names[key_less] = "key_less"
	c_names[key_more] = "key_more"
	c_names[key_ok] = "key_ok"

	for c=1,4 do
		controls[c] = AddVButton() 
		controls[c].gx = 0
		controls[c].x = controls[c].x + 32*c
		controls[c].y = 520
		controls[c].ax = 0
	end
	
	controls[key_less] = AddVButton()
	controls[key_more] = AddVButton()
	controls[key_ok] = AddVButton()
	
	cc = controls[key_less] 
	cc.ax = 0
	cc.y = 540
	cc.x = controls[1].x
	cc.w = cc.w *2 

	cc = controls[key_more] 
	cc.ax = 1
	cc.y = 540
	cc.x = controls[key_play].x + controls[key_play].w
	cc.w = cc.w * 2

	cc = controls[key_ok] 
	cc.ax = 0.5
	cc.y = 540
	cc.x = (controls[key_shift].x + controls[key_play].x+controls[key_play].w) / 2
	cc.w = cc.w * 2
	
	sx = controls[key_play].x + 24
	sy = S_HT - 4
	j = key_pad1
	for y=1,4 do
		for x=1,4 do
			controls[j] = AddVButton()
			controls[j].x = sx + x*24 
			controls[j].y = sy - (y-1)*20
			controls[j].ay = 1.0
			j = j + 1
		end
	end
	
	for c=1,23 do
		print(c_names[c])
		controls[c].tag = c_names[c]
	end	
	
	controls[key_stop].action = function ()
		print("transport.stop()")
	end
end