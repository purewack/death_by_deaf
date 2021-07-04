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
    audio.q_rec_clip(test_clip,0)
    audio.q_rec_clip(test_clip2,20)

    audio.q_rec_clip(test_clip,50)
    audio.q_rec_clip(test_clip2,50)
    
    audio.q_confirm()
end,3000,"test audio q")

visuals.onFrame = function()     
    local c = test_clip
    local bb = control.units[control.key_pad1]
    if(c.state == CLEAR) then bb.v_hue = 0; end
    if(c.state == STOP) then bb.v_hue = visuals.HUE_ORANGE; end
    if(c.state == BASE or c.state == DUB) then bb.v_hue = visuals.HUE_RED; end
    if(c.state == PLAY) then bb.v_hue = visuals.HUE_GREEN; end
    
    local c = test_clip2
    local bb = control.units[control.key_pad2]
    if(c.state == CLEAR) then bb.v_hue = 0; end
    if(c.state == STOP) then bb.v_hue = visuals.HUE_ORANGE; end
    if(c.state == BASE or c.state == DUB) then bb.v_hue = visuals.HUE_RED; end
    if(c.state == PLAY) then bb.v_hue = visuals.HUE_GREEN; end
end