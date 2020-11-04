#pragma once
#include "serializer.h"

struct Player {
  vector3 position;
  int state;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(Player));
    position.setDefaultValues();
  }

  bool equals(const Player & rhs) const {
    return
      position == rhs.position &&
      state == rhs.state;
  }
};

bool operator==(const Player &lhs, const Player &rhs) {
  return lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
//serializer                                                     //
///////////////////////////////////////////////////////////////////
inline void serialize(Player &o, ISerializer &s) {
  s.hint_type("Player");
  s.set_field_name("position");
  serialize(o.position, s);
  s.set_field_name("state");
  serialize(o.state, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
//deserializer                                                   //
///////////////////////////////////////////////////////////////////
inline void deserialize(Player &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next()) {
    switch (s.name_hash()) {
      case ros::hash("state"): deserialize(o.state, s); break;
      default: s.skip();
    }
  }
}

///////////////////////////////////////////////////////////////////
//hashing                                                        //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(Player &o) {
    ros::hash_value h = ros::hash(o.position);
    h = ros::xor64(h);
    h ^= ros::hash(o.state);
    return h;
  }
}
