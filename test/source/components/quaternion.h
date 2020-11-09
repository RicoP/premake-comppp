///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include "serializer.h"

struct quaternion {
  float w;
  float x;
  float y;
  float z;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(quaternion));
  }

  bool equals(const quaternion & rhs) const {
    return
      w == rhs.w &&
      x == rhs.x &&
      y == rhs.y &&
      z == rhs.z;
  }
};

inline bool operator==(const quaternion &lhs, const quaternion &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const quaternion &lhs, const quaternion &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(quaternion &o, ISerializer &s) {
  s.begin();
  s.key("w");
  serialize(o.w, s);
  s.key("x");
  serialize(o.x, s);
  s.key("y");
  serialize(o.y, s);
  s.key("z");
  serialize(o.z, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(quaternion &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("w"):
        deserialize(o.w, s);
        break;
      case ros::hash("x"):
        deserialize(o.x, s);
        break;
      case ros::hash("y"):
        deserialize(o.y, s);
        break;
      case ros::hash("z"):
        deserialize(o.z, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(quaternion &o) {
    ros::hash_value h = ros::hash(o.w);
    h = ros::xor64(h);
    h ^= ros::hash(o.x);
    h = ros::xor64(h);
    h ^= ros::hash(o.y);
    h = ros::xor64(h);
    h ^= ros::hash(o.z);
    return h;
  }
}
