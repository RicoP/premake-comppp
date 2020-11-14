///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include "serializer.h"

#include "components/objectid.h"
struct Skybox {
  ObjectID object;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(Skybox));
    object.setDefaultValues();
  }

  bool equals(const Skybox & rhs) const {
    return
      object == rhs.object;
  }
};

inline bool operator==(const Skybox &lhs, const Skybox &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const Skybox &lhs, const Skybox &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(Skybox &o, ISerializer &s) {
  s.begin("Skybox", ros::hash("Skybox"), &o);
  s.key("object");
  serialize(o.object, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(Skybox &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("object"):
        deserialize(o.object, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(Skybox &o) {
    ros::hash_value h = ros::hash(o.object);
    return h;
  }
}

///////////////////////////////////////////////////////////////////
// randomize                                                     //
///////////////////////////////////////////////////////////////////
inline void randomize(Skybox &o, ros::hash_value & h) {
  randomize(o.object, h);
}
