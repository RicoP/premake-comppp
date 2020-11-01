
local m = {}

m._VERSION = "0.1" -- dev version

local state = {}
state.objects = {}

function m.targetdir(dir)
	state.dir = dir
end

function m.new(name)
	print(name)
	state.current = name
end

function m.add(obj)
	state.objects[state.current] = obj
	print("add object to " .. state.name)
end

function m.generate()
	print("Generating...")
end

print("The comp++ module has loaded!")
return m