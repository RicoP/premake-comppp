#pragma once
#include <game/serializer.h>

struct vector3 {
  float x;
  float y;
  float z;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(vector3));
    z = 0;
  }
};

/////////////////////////////////////////////////////////////////////
//serializer                                                       //
/////////////////////////////////////////////////////////////////////
template <class SERIALIZER>
struct Serializer<vector3, SERIALIZER> {
  static bool serialize(vector3 &o, SERIALIZER &w) {
    w.write("x", "float", o.x);
    w.write("y", "float", o.y);
    w.write("z", "float", o.z);
    w.end();
    return true;
  }
};

/////////////////////////////////////////////////////////////////////
//deserializer                                                     //
/////////////////////////////////////////////////////////////////////
template <class DESERIALIZER>
struct Deserializer<vector3, DESERIALIZER> {
  static bool deserialize(vector3 &o, DESERIALIZER &r) {
    o.setDefaultValues();
    bool ok = false;
    do {
      if (!r.newLine()) return true;
      r.readHead();
      switch (field_hash(r.name, r.valtype)) {
        case field_hash("x", "float"): r.read(o.x); break;
        case field_hash("y", "float"): r.read(o.y); break;
        case field_hash("z", "float"): r.read(o.z); break;
        default: r.skip();
      }
      ok = r.ok();
    } while (ok && !r.finished());
    return ok;
  }
};
