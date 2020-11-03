local m = {}

m._VERSION = "0.1" -- dev version

local state = {}
state.objects = {}

local file = nil
local components = nil

--debug
function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end

--https://help.interfaceware.com/code/details/stringutil-lua
function trimRight(s)
   return s:match('^(.-)%s*$')
end

function write_no_new_line(s)
  file:write(trimRight(s))
end

function write(s)
  --print(s)
  file:write(trimRight(s), "\n")
end

-- int -> int
-- vector3 -> vector3
-- float[3] -> float
function vtype(type)
  if(type:match("%[%d+%]")) then
    return type:match "%a+"
  end
  return type
end

-- int -> int
-- vector3 -> vector3
-- float[3] -> float[]
function ctype(type)
  if(type:match("%[%d+%]")) then
    type = type:match "%a+"
    return type .. "[]"
  end
  return type
end

-- int -> 1
-- vector3 -> 1
-- float[3] -> 3
function size(type)
  if(type:match("%[%d+%]")) then
    return type:match("%d+")
  end
  return 1
end

-- int -> false
-- vector3 -> false
-- float[3] -> true
function vector(type)
  if(type:match("%[%d+%]")) then
    return true
  end
  return false
end

function execute()
  for struct, fieldsUnsorted in pairs(components) do
    local path = state.dir .. "/" .. struct .. ".h"
    print(struct .. " -> " .. path)
    file = io.open(path, "w")

    --print("fieldsUnsorted: ", dump(fieldsUnsorted))

    local fields = {}
    for k,v in pairs(fieldsUnsorted) do
      local e = {}
      table.insert(e, k)
      if type(v) == "string" then
        table.insert(e, v)
      else
        for i=1,#v do
          table.insert(e, v[i])
        end
      end
      table.insert(fields, e)
    end

    table.sort(fields, function (left, right)
        --TODO: sort not only by name but first by type size
        return left[1] < right[1]
    end)

    --print(dump(fields))

    -- STRUCT
    write("#pragma once")
    write('#include "serializer.h"')
    write("")
    write("struct " .. struct .. " {")
    for i=1,#fields do
      local name = fields[i][1]
      local valtype = fields[i][2]

      --print(name)
      --print(valtype)

      if vector(valtype) then
        write('  CArray<' .. size(valtype) .. ', ' .. vvaltype(valtype) .. '> ' .. name .. ';')
      else
        write('  ' .. valtype .. ' ' .. name .. ';')
      end
    end

    --default values
    write("")
    write("  void setDefaultValues() {")
    write("    std::memset(this, 0, sizeof(" .. struct .. "));")
    for i=1,#fields do
      local name = fields[i][1]
      local valtype = fields[i][2]
      local defaultValue = fields[i][3]

      if defaultValue then
        write("    " .. name .. " = " .. defaultValue .. ";")
      else
        if components[valtype] then
          -- feld in struct is one of our components. means it has a init method.
          write("    " .. name .. ".setDefaultValues();")
        end
        if vector(valtype) then
          write("    " .. name .. ".size = 0;")
        end
      end
    end
    write("  }")

    --equals
    write("")
    write('  bool equals(const '.. struct ..' & rhs) {      ')
    write('    return                                         ')
    write_no_new_line('      ')
    for i=1,#fields do
      local name = fields[i][1]

      if i ~= 1 then
        write(" &&")
      end
      write_no_new_line("      " .. name .. " == rhs." .. name)
    end
    write(";")
    write("  }")

    --struct end
    write('};')
    write('')
    --SERIALIZER
    write('///////////////////////////////////////////////////////////////////')
    write('//serializer                                                     //')
    write('///////////////////////////////////////////////////////////////////')
    write('inline void serialize('.. struct ..' &o, ISerializer &s) {         ')
    write('  s.hint_type("'.. struct ..'");                                   ')
    for i=1,#fields do
      local name = fields[i][1]
      local valtype = fields[i][2]
      local isComponent = components[valtype]
      if isComponent then
        --TODO: Do we need that?
        --write('  w.write_component("'.. name ..'", "'.. ctype(valtype) ..'", o.'.. name ..');')
        write('  s.set_field_name("'.. name ..'");                            ')
        write('  serialize(o.'.. name ..', s);                                ')
      else
        write('  s.set_field_name("'.. name ..'");                            ')
        write('  serialize(o.'.. name ..', s);                                ')
      end
    end
    write('  w.end();                                                         ')
    write('}                                                                  ')

    --DESERIALIZER
    write('')
    write('///////////////////////////////////////////////////////////////////')
    write('//deserializer                                                   //')
    write('///////////////////////////////////////////////////////////////////')
    write('inline bool deserialize('.. struct ..' &o, IDeserializer &r) {     ')
    write('  o.setDefaultValues();                                            ')
    write('  while (s.next()) {                                               ')
    write('    switch (s.name_hash()) {                                       ')
    for i=1,#fields do
      local name = fields[i][1]
      local valtype = fields[i][2]
      local isComponent = components[valtype]
      if isComponent then
        --TODO: does this need an extra step?
        --write('      case field_hash("' .. name ..'", "' .. ctype(valtype) ..'"): if(!r.finished()) { r.read_component(o.' .. name ..'); } break; ')
      else
        write('      case ros::hash("' .. name ..'"): deserialize(o.' .. name ..', s); break; ')
      end
    end
    write('      default: r.skip();                                           ')
    write('    }                                                              ')
    write('  }                                                                ')
    write('}                                                                  ')

    --DESERIALIZER
    write('')
    write('///////////////////////////////////////////////////////////////////')
    write('//hashing                                                        //')
    write('///////////////////////////////////////////////////////////////////')
    write('namespace ros {                                                    ')
    write('  inline ros::hash_value hash('.. struct ..' &o) {     ')
    write_no_new_line('    ros::hash_value h =                                ')
    for i=1,#fields do
      local name = fields[i][1]
      if i == 1 then
        write(' ros::hash(o.'.. name ..');                                    ')
      else
        write('    h = ros::xor64(h);                                         ')
        write('    h ^= ros::hash(o.' .. name ..');                           ')
      end
    end
    if #fields == 0 then
        write(' 0;                                                            ')
    end
    write('    return h;                                                      ')
    write('  }                                                                ')
    write('}                                                                  ')

    file:close()
  end
end

function m.targetdir(dir)
  state.dir = dir
end

function m.new(name)
  print(name)
  state.current = name
end

function m.add(obj)
  state.objects[state.current] = obj
  components = state.objects
  print("add object to " .. state.current)
end

function m.generate()
  print("Generating...")
  execute()
end

print("The comp++ module has loaded!")
return m





















