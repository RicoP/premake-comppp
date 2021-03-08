local tsort = require "tsort"

local m = {}

m._VERSION = "0.1" -- dev version

local state = {}
state.objects = {}
state.enums = {}

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

function keys(tab)
  local keyset = {}
  for k,v in pairs(tab) do
    keyset[#keyset + 1] = k
  end
  table.sort(keyset)
  return keyset
end

--https://gist.github.com/FGRibreau/3790217
function tab_filter(t, filterIter)
  local out = {}
  print(t)
  for k, v in pairs(t) do
    if filterIter(v, k, t) then out[k] = v end
  end
  return out
end

function is_non_primitive(v)
  --return components[v].__include == nil
  --HACK primitive types are always lowercase?
  if v == "TextureAssetRef" then return false end
  if v == "ShaderAssetRef"  then return false end
  if v == "SoundAssetRef"   then return false end
  if v == "MeshAssetRef"    then return false end
  if v == "ObjectID"        then return false end
  if v == v:lower()         then return false end
  return true
end

function is_primitive(v)
  return not is_non_primitive(v)
end

--https://help.interfaceware.com/code/details/stringutil-lua
function trimRight(s)
   return s:match('^(.-)%s*$')
end

--https://stackoverflow.com/a/2421746
function firstToUpper(str)
    return (str:gsub("^%l", string.upper))
end

--https://stackoverflow.com/a/19263313
--[[Holy shit. Lua dosn't even has a fucking split function. Now I have to copy and paste some stupid shit
like that from SO because in their infinite wisdom, the creators of Lua decited that all string operations
shall happen with regular expressions, and simple everyday life functions like a simple split are is simply
beneath their precious time. WTF?!]]
function string:split( inSplitPattern, outResults )
  if not outResults then
    outResults = { }
  end
  local theStart = 1
  local theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
  while theSplitStart do
    table.insert( outResults, string.sub( self, theStart, theSplitStart-1 ) )
    theStart = theSplitEnd + 1
    theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
  end
  table.insert( outResults, string.sub( self, theStart ) )
  return outResults
end

function write_no_new_line(s)
  file:write(trimRight(s))
end

function write(s)
  --print(s)
  file:write(trimRight(s), "\n")
end

function write_autogen(s)
  write('///////////////////////////////////////////////////////////////////')
  write('// AUTOGENERATED do not edit by hand                             //')
  write('///////////////////////////////////////////////////////////////////')
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
assert(vtype("int") == "int")
assert(vtype("vector3") == "vector3")
assert(vtype("float[3]") == "float")

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
assert(ctype("int") == "int")
assert(ctype("vector3") == "vector3")
assert(ctype("float[3]") == "float[]")

-- int -> 1
-- vector3 -> 1
-- float[3] -> 3
function size(type)
  local s = type:match("%[%d+%]");
  if s then
    return tonumber(s:match("%d+"))
  end
  return 1
end
assert(size("int") == 1)
assert(size("vector3") == 1)
assert(size("float[3]") == 3)


-- int -> false
-- vector3 -> false
-- float[3] -> true
function vector(type)
  return type:match("%[%d+%]") ~= nil
end
assert(vector("int") == false)
assert(vector("vector3") == false)
assert(vector("float[3]") == true)

-- int -> false
-- vector3 -> false
-- Action|Blub -> true
function enum(type)
  return type:match("|") ~= nil
end
assert(enum("int") == false)
assert(enum("vector3") == false)
assert(enum("Action|Blub") == true)

-- int -> 0
-- vector3 -> 0
-- float[3] -> 0
-- string(3) -> 3
-- string(1024) -> 1024
function string_type(type)
  local s = type:match("string%(%d+%)")
  if s ~= nil then
    return tonumber(s:match("%d+"))
  end
  return 0
end
assert(string_type("int") == 0)
assert(string_type("vector3") == 0)
assert(string_type("float[3]") == 0)
assert(string_type("string(3)") == 3)
assert(string_type("string(1024)") == 1024)

function execute()
  assert(components.component == nil, "Can't have a component called component")
  assert(components.components == nil, "Can't have a component called components")

  for struct, fieldsUnsorted in pairs(components) do
    local path = state.dir .. "/" .. struct:lower() .. ".h"
    print(struct .. " -> " .. path)
    file = io.open(path, "w")

    local include = fieldsUnsorted["__include"]
    fieldsUnsorted["__include"] = nil

    if not include then
      assert(fieldsUnsorted["entity"] == nil, "can't have a field called entity")
      fieldsUnsorted["entity"] = {"rose::ecs::Entity"}
    end

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

    components[struct] = fields

    table.sort(fields, function (left, right)
        --TODO: sort not only by name but first by type size
        return left[1] < right[1]
    end)

    -- STRUCT
    write_autogen()
    write('#pragma once                                                       ')
    write('#include <serializer/serializer.h>                                 ')
    write('#include <rose/ecs.h>                                              ')
    write('#include <cstring>                                                 ')
    write('                                                                   ')

    for _,compA in pairs(fields) do
      local comp = vtype(compA[2])
      local is_component = components[comp] ~= nil
      if is_component then
        write('#include <game/components/' .. comp:lower() .. '.h>')
      end
    end

    if include == nil then
      write("struct " .. struct .. " {")
      for i=1,#fields do
        local name = fields[i][1]
        local valtype = fields[i][2]

        if enum(valtype) then
          write('  enum class ' .. firstToUpper(name) .. ' : long long {                        ')
          write('    None = 0,                                                                  ')
          local enum_member = valtype:split("|")
          for i,v in pairs(enum_member) do
            write('    '.. v ..' = 1 << ' .. i-1 .. ',                                          ')
          end
          write('  };                                                                           ')
          write('  ' .. firstToUpper(name) .. ' ' .. name .. ';')
        elseif string_type(valtype) ~= 0 then
          write('  ros::string<' .. string_type(valtype) .. '> ' .. name .. ';')
        elseif vector(valtype) then
          write('  rose::vectorPOD<' .. size(valtype) .. ', ' .. vtype(valtype) .. '> ' .. name .. ';')
        else
          write('  ' .. valtype .. ' ' .. name .. ';')
        end
      end

      --equals
      write("")
      write('  bool equals(const '.. struct ..' & rhs) const {                  ')
      write('    return                                                         ')
      write_no_new_line('                                                       ')
      for i=1,#fields do
        local name = fields[i][1]

        if i ~= 1 then
          write(" &&")
        end
        write_no_new_line("      " .. name .. " == rhs." .. name)
      end
      write(';                                                                  ')
      write('  }                                                                ')
      write('};                                                                 ')
      --default values
      write("")
      write("inline void construct_defaults(".. struct .." & o) {                    ")
      write("  std::memset(&o, 0, sizeof(" .. struct .. "));                  ")
      for i=1,#fields do
        local name = fields[i][1]
        local valtype = fields[i][2]
        local defaultValue = fields[i][3]

        if defaultValue then
          write("  o." .. name .. " = " .. defaultValue .. ";                 ")
        else
          if components[valtype] then
            -- feld in struct is one of our components. means it has a init method.
            write("  construct_defaults(o." .. name .. ");                    ")
          end
          if vector(valtype) then
            write("  o." .. name .. ".size = 0;                               ")
          end
        end
      end
      write("}")

      write('')
      --struct end

      write('inline bool operator==(const '.. struct ..' &lhs, const '.. struct ..' &rhs) { ')
      write('  return lhs.equals(rhs);                                                      ')
      write('}                                                                              ')
      write('')
      write('inline bool operator!=(const '.. struct ..' &lhs, const '.. struct ..' &rhs) { ')
      write('  return !lhs.equals(rhs);                                                     ')
      write('}                                                                              ')
    else
      write("")
      write("#include <" .. include .. ">")
      write("")
    end

    --ENUM OPERATOS
    for i=1,#fields do
      local name = fields[i][1]
      local valtype = fields[i][2]

      if enum(valtype) then
        local E = struct .. "::" .. firstToUpper(name)
        write('inline ' .. E .. ' operator|(' .. E .. ' lhs, ' .. E .. ' rhs) {                              ')
        write('  return (' .. E .. ')((long long)(lhs) | (long long)(rhs));                                  ')
        write('}                                                                                             ')
        write('                                                                                              ')
        write('inline ' .. E .. ' operator&(' .. E .. ' lhs, ' .. E .. ' rhs) {                              ')
        write('  return (' .. E .. ')((long long)(lhs) & (long long)(rhs));                                  ')
        write('}                                                                                             ')
        write('                                                                                              ')
        write('inline ' .. E .. ' operator&=(' .. E .. ' & lhs, ' .. E .. ' rhs) { return lhs = lhs & rhs; } ')
        write('inline ' .. E .. ' operator|=(' .. E .. ' & lhs, ' .. E .. ' rhs) { return lhs = lhs | rhs; } ')
        write('                                                                                              ')
        write('inline void serialize(' .. E .. ' &o, ISerializer &s) {                                       ')
        local enum_member = valtype:split("|")
        for i,v in pairs(enum_member) do
          write('  if ((o & ' .. E .. '::' .. v .. ') != ' .. E .. '::None) s.write_enum("' .. v .. '");     ')
        end

        write('  s.end_enum();                                                                               ')
        write('}                                                                                             ')
        write('                                                                                              ')

        write('inline void deserialize(' .. E .. ' &o, IDeserializer &d) {                                   ')
        write('  o = ' .. E .. '::None;                                                                      ')
        write('  while (d.in_enum()) {                                                                       ')
        write('    switch (d.hash_key()) {                                                                   ')
        for i,v in pairs(enum_member) do
          write('      case ros::hash("' .. v .. '"):                                                        ')
          write('        o |= ' .. E .. '::' .. v .. ';                                                      ')
          write('        break;                                                                              ')
        end
        write('      default:                                                                                ')
        write('        break;                                                                                ')
        write('    }                                                                                         ')
        write('  }                                                                                           ')
        write('}                                                                                             ')
        write('')

        write('inline void randomize(' .. E .. ' &o, ros::hash_value & h) {                                                       ')
        write('  h = ros::xor64(h); ')
        write('  o = ' .. E .. '::None;                                                                      ')
        write('  switch(h % ' .. #enum_member .. ') {                                                        ')
        for i,v in pairs(enum_member) do
          write('    case ' .. (i-1) .. ': o = ' .. E .. '::' .. v .. '; break;                              ')
        end
        write('  }                                                                                           ')
        write('}                                                                                             ')
        write('')

      end
    end

    write('')
    write('///////////////////////////////////////////////////////////////////     ')
    write('// serializer                                                    //     ')
    write('///////////////////////////////////////////////////////////////////     ')
    write('inline void serialize('.. struct ..' &o, ISerializer &s) {              ')
    write('  if(s.node_begin("'.. struct ..'", ros::hash("'.. struct ..'"), &o)) { ')
    for i=1,#fields do
      local name = fields[i][1]
      write('    s.key("'.. name ..'");                                              ')
      write('    serialize(o.'.. name ..', s);                                       ')
    end
    write('    s.node_end();                                                       ')
    write('  }                                                                     ')
    write('  s.end();                                                              ')
    write('}                                                                       ')

    write('')
    write('///////////////////////////////////////////////////////////////////')
    write('// deserializer                                                  //')
    write('///////////////////////////////////////////////////////////////////')
    write('inline void deserialize('.. struct ..' &o, IDeserializer &s) {     ')
    write('  construct_defaults(o);                                           ')
    write('                                                                   ')
    write('  while (s.next_key()) {                                           ')
    write('    switch (s.hash_key()) {                                        ')
    for i=1,#fields do
      local name = fields[i][1]
      write('      case ros::hash("' .. name ..'"):                           ')
      write('        deserialize(o.' .. name ..', s);                         ')
      write('        break;                                                   ')
    end
    write('      default: s.skip_key(); break;                                ')
    write('    }                                                              ')
    write('  }                                                                ')
    write('}                                                                  ')

    write('')
    write('///////////////////////////////////////////////////////////////////')
    write('// hashing                                                       //')
    write('///////////////////////////////////////////////////////////////////')
    write('namespace ros {                                                    ')
    write('  inline ros::hash_value hash('.. struct ..' &o) {     ')
    write_no_new_line('    ros::hash_value h =                                ')
    for i=1,#fields do
      local name = fields[i][1]
      local valtype = fields[i][2]
      if i == 1 then
        write(' ros::hash(o.'.. name ..');                                    ')
      else
        write('    h = ros::xor64(h);                                         ')
        if enum(valtype) then
          write('    h ^= ros::hash((long long)o.' .. name ..');              ')
        else
          write('    h ^= ros::hash(o.' .. name ..');                         ')
        end
      end
    end
    if #fields == 0 then
        write(' 0;                                                            ')
    end
    write('    return h;                                                      ')
    write('  }                                                                ')
    write('}                                                                  ')

    write('')
    write('///////////////////////////////////////////////////////////////////')
    write('// randomize                                                     //')
    write('///////////////////////////////////////////////////////////////////')
    write('inline void randomize('.. struct ..' &o, ros::hash_value & h) {    ')
    for i,field in pairs(fields) do
      local name = field[1]
      write('  randomize(o.'.. name ..', h);                                  ')
    end
    write('}                                                                  ')

    file:close()
  end

  local graph = tsort.new()
  for struct, fields in pairs(components) do
    --print(struct)
    for _,compA in pairs(fields) do
      --print("  ", dump(compA))
      local comp = vtype(compA[2])
      local is_component = components[comp] ~= nil
      if is_component then
        --print(struct, "->", comp)
        graph:add(comp, struct)
      end
    end
  end
  --print(dump(graph))
  --print(dump(graph:sort()))
  local path = state.dir .. "/components.h"
  file = io.open(path, "w")
  write_autogen()
  write('')
  write('#pragma once')
  write('')
  for _,struct in pairs(graph:sort()) do
    write('#include "components/' .. struct:lower() .. '.h"                 ')
  end
end

function create_world(name, comps)
  if comps == nil then
    comps = tab_filter(keys(components), is_non_primitive)
  end

  function pluralize(name)
    return name:lower() .. "s"
  end

  local path = state.dir .. "/" .. name:lower() .. ".h"
  print("world ", struct, " -> ", path)
  file = io.open(path, "w")

  write_autogen()
  write('')
  write('#pragma once')
  write("")
  write("#include <game/components/entity.h>                                                                 ")
  for k,name in pairs(comps) do
    write("#include <game/components/" .. name:lower() .. ".h> ")
  end
  write("#include <rose/ecs.h>                                                                             ")
  write("#include <vector>                                                                             ")
  write("")
  write("struct World : rose::ecs::World {                                                                        ")
  write("")
  write("  //component indice                                                                                ")
  for k,name in pairs(comps) do
    --write("  std::vector<int> camera_index; //  = {-1};                                                        ")
    write("  std::vector<int> " .. name:lower() .. "_index; ")
  end
  write("")
  for k,name in pairs(comps) do
    write("  std::vector<" .. name .. "> " .. pluralize(name) .. "; ")
  end

  write("")
  write("  rose::ecs::Entity create_entity() {                      ")
  write("    // Assuming arrays have no holes                       ")
  write("    int index = entities.size();                           ")
  write("    rose::ecs::Entity& entity = entities.emplace_back();   ")
  write("    entity.generation = ~0;                                ")
  write("    entity.index = index;                                  ")
  write("    generation.push_back(entity.generation);               ")
  write("    status.push_back(rose::ecs::Entity::Status::Alive);    ")
  write("")
  for k,name in pairs(comps) do
    local lname = name:lower()
    local names = pluralize(name)
    local name_index = lname .. "_index"
    write("    " .. name_index .. ".push_back(-1);                  ")
  end
  write("")
  write("    return entity;                                         ")
  write("  }")

  write("")
  -- TODO
  write("  //WorldSystem worlsSystem;                                                       ")
  write("  //RenderSystem renderSystem;                                                     ")
  write("                                                                                   ")
  write("  //Component getter                                                               ")
  write("  template<typename T>                                                             ")
  write("  T & get(rose::ecs::Entity entity); //Not implemented -> unknown type             ")
  write("  template<typename T>                                                             ")
  write("  const T & get(rose::ecs::Entity entity) const; //Not implemented -> unknown type ")
  write("  template <typename T>                                                            ")
  write("  T& attach_component(rose::ecs::Entity entity);                                   ")

  for k,name in pairs(comps) do
    if is_primitive(name) then goto continue end
    local lname = name:lower()
    local names = pluralize(name)
    local name_index = lname .. "_index"
    write("")
    write("  template <> [[nodiscard]] " .. name .. "& attach_component<" .. name .. ">(rose::ecs::Entity entity) {                      ")
    write("    return attach_component_to_vector(" .. name_index .. ", " .. names .. ", entity);                             ")
    write("  }                                                                                                     ")
    write("")
    write("  // get " .. name)
    write("  template <> [[nodiscard]] " .. name .. "& get<" .. name .. ">(rose::ecs::Entity entity) {             ")
    write("    return get_component(" .. name_index .. ", " .. names .. ", entity);                                ")
    write("  }                                                                                                     ")
    write("")
    write("  template <> [[nodiscard]] const " .. name .. "& get<" .. name .. ">(rose::ecs::Entity entity) const { ")
    write("    return get_component(" .. name_index .. ", " .. names .. ", entity);                                ")
    write("  }                                                                                                     ")
    ::continue::
  end
  write("};")

  print(comps)
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

function m.add_value_type(include, obj)
  obj.__include = include
  state.objects[state.current] = obj
  components = state.objects
  print("add object to " .. state.current)
end

function m.world(name, comps)
  create_world(name, comps)
end

function m.generate()
  print("Generating...")
  execute()
end

print("The comp++ module has loaded!")
return m
