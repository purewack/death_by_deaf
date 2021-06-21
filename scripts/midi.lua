ev_note_on = 0x90
ev_note_cc = 0xA0
map = {}
map[ev_note_on] = {}
map[ev_note_cc] = {}

sesh = root
function readJSON()
	local file = io.open(sesh .. "mappings.json","r")
	read_json = json.decode(file:read "*a")
	io.close(file)
end

function writeJSON()
	local file = io.open(sesh .. "mappings.json","w")
	local to_write = json.encode(read_json)
	io.output(file)
	io.write(to_write)
	io.close(file)
end

function reloadMidi()
	for x in pairs(read_json) do
		ev = read_json[x]
		if ev["event"] == ev_note_on then
			map[ev_note_on][ev["event_key"]] = load(ev["event_action"])
		end
	end
end

function remapMidi(ev,note,vel,code) 

end

function checkMidi(w,ev,note,vel)
	action = map[ev][note]
	if action and vel > 0 then
		ev_vel = vel
		action()
	end
end

readJSON()
reloadMidi()
