controls = {}
control_names = {}
key_shift = 1
key_stop = 2
key_rec = 3
key_play = 4

key_pad1 = 5
key_pad2 = 6
key_pad3 = 7
key_pad4 = 8

key_pad5 = 9
key_pad6 = 10
key_pad7 = 11
key_pad8 = 12

key_pad9 = 13
key_pad10 = 14
key_pad11 = 15
key_pad12 = 16

key_pad13 = 17
key_pad14 = 18
key_pad15 = 19
key_pad16 = 20

key_ok = 21
key_less = 22
key_more = 23

print("root: "..root)
loadfile(root .. "scripts/midi.lua")()

LoadFont("gfx/fixedsys.ttf")
LoadFont("gfx/Consolas.ttf")
LoadFont("gfx/AnonymousPro-Regular.ttf")
LoadFont("gfx/DroidSerif.ttf")
LoadFont("gfx/Lekton-Regular.ttf")
UIFont(0)

Actions(true)
Bench(true)
Chain(true)
Midi(true)
--Grid(true)
--GridDiv(16)


onUIReload = function () 
	
	control_names[key_shift] = "key_shift"
	control_names[key_stop] = "key_stop"
	control_names[key_rec] = "key_rec"
	control_names[key_play] = "key_play"

	for p=1,16 do
	control_names[key_pad1+p-1] = "key_pad_" .. p
	end
	
	control_names[key_less] = "key_less"
	control_names[key_more] = "key_more"
	control_names[key_ok] = "key_ok"

	for c=1,4 do
		controls[c] = AddVUnitButton(c) 
		controls[c].gx = 0
		controls[c].x = controls[c].x + 32*c
		controls[c].y = 550
		controls[c].ax = 0
	end
	
	controls[key_less] = AddVUnitButton(key_less)
	controls[key_more] = AddVUnitButton(key_more)
	controls[key_ok] = AddVUnitButton(key_ok)
	
	cc = controls[key_less] 
    cc.type = 176
	cc.ax = 0
	cc.y = 530
	cc.x = controls[1].x
	cc.w = cc.w *2 

	cc = controls[key_more]
    cc.type = 176 
	cc.ax = 1
	cc.y = 530
	cc.x = controls[key_play].x + controls[key_play].w
	cc.w = cc.w * 2

	cc = controls[key_ok] 
	cc.ax = 0.5
	cc.y = 530
	cc.x = (controls[key_shift].x + controls[key_play].x+controls[key_play].w) / 2
	cc.w = cc.w * 2
	
	sx = controls[key_play].x + 24
	sy = S_HT - 4
	j = key_pad1
	for y=1,4 do
		for x=1,4 do
			controls[j] = AddVUnitButton(j)
			controls[j].x = sx + x*20 
			controls[j].y = sy - (y-1)*20
			controls[j].ay = 1.0
			j = j + 1
		end
	end
	
	for c=1,23 do
		controls[c].tag = control_names[c]
	end	

end