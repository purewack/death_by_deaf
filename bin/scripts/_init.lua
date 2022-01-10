local control_names = {}
control.key_shift = 1
control.key_stop = 2
control.key_rec = 3
control.key_play = 4

control.key_pad1 = 5
control.key_pad2 = 6
control.key_pad3 = 7
control.key_pad4 = 8

control.key_pad5 = 9
control.key_pad6 = 10
control.key_pad7 = 11
control.key_pad8 = 12

control.key_pad9 = 13
control.key_pad10 = 14
control.key_pad11 = 15
control.key_pad12 = 16

control.key_pad13 = 17
control.key_pad14 = 18
control.key_pad15 = 19
control.key_pad16 = 20

control.key_ok = 21
control.key_less = 22
control.key_more = 23

system.loadFont("gfx/fixedsys.ttf")
system.loadFont("gfx/Consolas.ttf")
system.loadFont("gfx/AnonymousPro-Regular.ttf")
system.loadFont("gfx/DroidSerif.ttf")
system.loadFont("gfx/Lekton-Regular.ttf")
visuals.uiFont(0)
visuals.actions(true)
visuals.chain(true)
visuals.aque(true)
visuals.bench(true)
visuals.midi(true)


visuals.onUIReload = function () 
	
	control_names[control.key_shift] = "control.key_shift"
	control_names[control.key_stop] = "control.key_stop"
	control_names[control.key_rec] = "control.key_rec"
	control_names[control.key_play] = "control.key_play"

	for p=1,16 do
	control_names[control.key_pad1+p-1] = "control.key_pad_" .. p
	end
	
	control_names[control.key_less] = "control.key_less"
	control_names[control.key_more] = "control.key_more"
	control_names[control.key_ok] = "control.key_ok"

	for c=1,4 do
		control.units[c] = visuals.addVUnitButton(c) 
		control.units[c].gx = 0
		control.units[c].x = control.units[c].x + 32*c
		control.units[c].y = 550
		control.units[c].ax = 0
	end
	
	control.units[control.key_less] = visuals.addVUnitButton(control.key_less)
	control.units[control.key_more] = visuals.addVUnitButton(control.key_more)
	control.units[control.key_ok] = visuals.addVUnitButton(control.key_ok)
	
	cc = control.units[control.key_less] 
    cc.type = 176
	cc.ax = 0
	cc.y = 530
	cc.x = control.units[1].x
	cc.w = cc.w *2 

	cc = control.units[control.key_more]
    cc.type = 176 
	cc.ax = 1
	cc.y = 530
	cc.x = control.units[control.key_play].x + control.units[control.key_play].w
	cc.w = cc.w * 2

	cc = control.units[control.key_ok] 
	cc.ax = 0.5
	cc.y = 530
	cc.x = (control.units[control.key_shift].x + control.units[control.key_play].x+control.units[control.key_play].w) / 2
	cc.w = cc.w * 2
	
	sx = control.units[control.key_play].x + 24
	sy = S_HT - 4
	j = control.key_pad1
	for y=1,4 do
		for x=1,4 do
			control.units[j] = visuals.addVUnitButton(j)
			control.units[j].x = sx + x*20 
			control.units[j].y = sy - (y-1)*20
			control.units[j].ay = 1.0
			j = j + 1
		end
	end
	
	for c=1,23 do
		control.units[c].tag = control_names[c]
	end	
    
    print("_init.lua: unit setup end")
end
