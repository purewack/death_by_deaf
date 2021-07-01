current_json = root .. "mappings.json"

function readJSON()
    local file = io.open(current_json,"r")
    read_json = json.decode(file:read "*a")
    io.close(file)
end

function writeJSON()
    local file = io.open(current_json,"w")
    local to_write = json.encode(read_json)
    io.output(file)
    io.write(to_write)
    io.close(file)
end

function reloadMidi()
    for x in pairs(read_json) do
        ev = read_json[x]
        if ev["event"] == control.ev_note_on then
            if ev["device"] == 0 then
                control.unitmap[control.ev_note_on][ev["event_key"]] = load(ev["event_action"])
                print("unit on->" .. ev["event_action"])
                
                if ev["event_ending"] then
                control.unitmap[control.ev_note_off][ev["event_key"]] = load(ev["event_ending"])
                print("unit off->" .. ev["event_ending"])
                end
            else
                control.map[control.ev_note_on][ev["event_key"]] = load(ev["event_action"])
                print("ext on->" .. ev["event_action"])
                
                if ev["event_ending"] then
                control.map[control.ev_note_off][ev["event_key"]] = load(ev["event_ending"])
                print("ext off->" .. ev["event_ending"])
                end
            end
        end
    end
end

function remapMidi(w,ev,note,code)

end

function checkMidi(w,ev,note,vel)
    
    if ev == control.ev_note_on and vel == 0 then
        ev = control.ev_note_off
    end

    if w == 0 then
    action = control.unitmap[ev][note]
    else
    action = control.map[ev][note]
    end

    if action then
        control.ev_vel = vel
        action()
    end

end

readJSON()
reloadMidi()
