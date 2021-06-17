ev_note_on = 0x90
ev_note_cc = 0xA0
map = {}
map[ev_note_on] = {}
map[ev_note_cc] = {}

sesh = " "
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

function remapMidi()
	for x in pairs(read_json) do
		ev = read_json[x]
		if ev["event"] == ev_note_on then
			map[ev_note_on][ev["event_key"]] = load(ev["event_action"])
		end
	end
end

function checkMidi(e,n)
	action = map[e][n]
	if action then
		action()
	end
end
