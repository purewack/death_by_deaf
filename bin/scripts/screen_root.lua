local n_on = control.ev_note_on
local n_off = control.ev_note_off
local n_cc = control.ev_note_cc
-- control.unitmap[n_on][control.key_shift] = function()
--     system.shift = 1
-- end
-- control.unitmap[n_off][control.key_shift] = function()
--     system.shift = 0
-- end

-- control.unitmap[n_on][control.key_stop] = function()
--     if not sesh.stopped then
--         audio.stop()
--     else
--         system.async_after(function()
--             if control.units[control.key_stop].state then
--                audio.clearAll() 
--             end
--         end,1500,"stop_hold")
--     end
-- end

-- system.async_after(function()
--     audio.que.test()
--     audio.que.test()
--     audio.que.confirm()
--     print(audio.que.tick())
-- end,1500,"test")

-- control.unitmap[n_on][control.key_stop] = function()
--     audio.que.stop(test_clip,0.0)
--     audio.que.confirm()
-- end

-- control.unitmap[n_on][control.key_rec] = function()
--     audio.que.launch(test_clip,0.0)
--     audio.que.confirm()
-- end

-- control.unitmap[n_on][control.key_pad1] = function()
--     audio.que.launch(test_clip,0.0)
--     audio.que.confirm()
-- end

-- prog = visuals.addVTimer()
-- prog.x = 320
-- prog.y = 200
-- prog.w = 200
-- prog.circular = false

-- lbl_c_state = visuals.addVLabel("")
-- lbl_c_state.x = 320
-- lbl_c_state.y = 260

-- local aa = 0

-- visuals.onFrame = function()    
--     -- local c = test_clip
--     -- local bb = control.units[control.key_pad1]
--     -- if(c.isClear) then bb.v_hue = 0; end
--     -- if(c.isStopped) then bb.v_hue = visuals.HUE_ORANGE; end
--     -- if(c.isPlaying) then bb.v_hue = visuals.HUE_GREEN; end
--     -- if(c.isRecording) then bb.v_hue = visuals.HUE_RED; end
--     -- if(c.isMerging) then bb.v_hue = visuals.HUE_PURPLE; end

--     -- local c = test_clip2
--     -- local bb = control.units[control.key_pad2]
--     -- if(c.isClear) then bb.v_hue = 0; end
--     -- if(c.isStopped) then bb.v_hue = visuals.HUE_ORANGE; end
--     -- if(c.isPlaying) then bb.v_hue = visuals.HUE_GREEN; end
--     -- if(c.isRecording) then bb.v_hue = visuals.HUE_RED; end
--     -- if(c.isMerging) then bb.v_hue = visuals.HUE_PURPLE; end
    
--     -- local t = audio.que.progress()
--     -- local ct = audio.que.tick()
    
--     prog.progress = aa/100
--     aa = aa+1
--     aa = aa%100
--     lbl_c_state.text = tostring(aa)
-- end

vignette = visuals.addVImage()
vig_tex = visuals.createTexture("gfx/vig.png")
vignette.tex = vig_tex
vignette.x = 0
vignette.y = 0
vignette.w = 800
vignette.h = 600
vignette.ax = 0.0
vignette.ay = 0.0
