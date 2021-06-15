local key_shift = 1
local key_stop = 2
local key_rec = 3
local key_play = 4
local key_less = 21
local key_ok = 20
local key_more = 22

controls = {}

for c=1,4 do
controls[c] = AddVButton() 
local cc = controls[c]
cc.gx = 0
cc.x = cc.x + 32*c
cc.y = 520
cc.ax = 0
end

controls[key_less] = AddVButton()
cc = controls[key_less] 
cc.ax = 0
cc.y = 540
cc.x = controls[1].x
cc.w = cc.w *2 

controls[key_more] = AddVButton()
cc = controls[key_more] 
cc.ax = 1
cc.y = 540
cc.x = controls[key_play].x + controls[key_play].w
cc.w = cc.w * 2

controls[key_ok] = AddVButton()
cc = controls[key_ok] 
cc.ax = 0.5
cc.y = 540
cc.x = (controls[key_shift].x + controls[key_play].x+controls[key_play].w) / 2
cc.w = cc.w * 2

--cc = controls[key_play]
--for yy=1,4 do
--	for xx=1,4 do
--		idx = xx + yy*4
--		controls[idx] = AddVButton()
--		cp = controls[idx]
--		cp.x = 
--	end
--end	