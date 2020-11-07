#pragma once
#include "serializer.h"

struct vector3 {
  float x;
  float y;
  float z;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(vector3));
    z = 0;
  }

  bool equals(const vector3 & rhs) const {
    return
      x == rhs.x &&
      y == rhs.y &&
      z == rhs.z;
  }
};

bool operator==(const vector3 &lhs, const vector3 &rhs) {
  return lhs.equals(rhs);
}

bool operator!=(const vector3 &lhs, const vector3 &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
//serializer                                                     //
///////////////////////////////////////////////////////////////////
inline void serialize(vector3 &o, ISerializer &s) {
  s.begin();
  s.key("x");
  serialize(o.x, s);
  s.key("y");
  serialize(o.y, s);
  s.key("z");
  serialize(o.z, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
//deserializer                                                   //
///////////////////////////////////////////////////////////////////
inline void deserialize(vector3 &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("x"): deserialize(o.x, s); break;
      case ros::hash("y"): deserialize(o.y, s); break;
      case ros::hash("z"): deserialize(o.z, s); break;
      default: s.skip_key();
    }
  }
}

///////////////////////////////////////////////////////////////////
//hashing                                                        //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(vector3 &o) {
    ros::hash_value h = ros::hash(o.x);
    h = ros::xor64(h);
    h ^= ros::hash(o.y);
    h = ros::xor64(h);
    h ^= ros::hash(o.z);
    return h;
  }
}
