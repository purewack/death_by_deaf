local n_on = control.ev_note_on
local n_off = control.ev_note_off
local n_cc = control.ev_note_cc
control.unitmap[n_on][control.key_shift] = function()
    system.shift = 1
end
control.unitmap[n_off][control.key_shift] = function()
    system.shift = 0
end

control.unitmap[n_on][control.key_stop] = function()
    if not sesh.stopped then
        audio.stop()
    else
        system.async_after(function()
            if control.units[control.key_stop].state then
               audio.clearAll() 
            end
        end,1500,"stop_hold")
    end
end
control.unitmap[n_off][control.key_stop] = function()
    system.async_cancel("stop_hold");
end


system.async_after(function()
    audio.que.clip.rec(test_clip,1.0)
    audio.que.clip.rec(test_clip2,1.5)
    audio.que.clip.rec(test_clip,2.0)
    audio.que.clip.rec(test_clip2,4.0)
    audio.que.confirm()
end,500,"test")

prog = visuals.addVTimer()
prog.x = 320
prog.y = 200
prog.w = 200
prog.circular = false
lbl_q_period = visuals.addVLabel("")
lbl_q_period.x = 320
lbl_q_period.y = 240
lbl_q_count = visuals.addVLabel("")
lbl_q_count.x = 320
lbl_q_count.y = 260

audio.que.period(500)

visuals.onFrame = function()    
    local c = test_clip
    local bb = control.units[control.key_pad1]
    if(c.isClear) then bb.v_hue = 0; end
    if(c.isStopped) then bb.v_hue = visuals.HUE_ORANGE; end
    if(c.isPlaying) then bb.v_hue = visuals.HUE_GREEN; end
    if(c.isRecording) then bb.v_hue = visuals.HUE_RED; end
    if(c.isMerging) then bb.v_hue = visuals.HUE_PURPLE; end

    local c = test_clip2
    local bb = control.units[control.key_pad2]
    if(c.isClear) then bb.v_hue = 0; end
    if(c.isStopped) then bb.v_hue = visuals.HUE_ORANGE; end
    if(c.isPlaying) then bb.v_hue = visuals.HUE_GREEN; end
    if(c.isRecording) then bb.v_hue = visuals.HUE_RED; end
    if(c.isMerging) then bb.v_hue = visuals.HUE_PURPLE; end
    
    local t = audio.que.progress()
    local ct = audio.que.count()
    prog.progress = t
    lbl_q_period.text = tostring(t)
    lbl_q_count.text = tostring(ct)
end