#pragma once
#include "serializer.h"

struct World {
  ros::array<4, Player> position;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(World));
    position.size = 0;
  }

  bool equals(const World & rhs) const {
    return
      position == rhs.position;
  }
};

bool operator==(const World &lhs, const World &rhs) {
  return lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
//serializer                                                     //
///////////////////////////////////////////////////////////////////
inline void serialize(World &o, ISerializer &s) {
  s.hint_type("World");
  s.set_field_name("position");
  serialize(o.position, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
//deserializer                                                   //
///////////////////////////////////////////////////////////////////
inline void deserialize(World &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next()) {
    switch (s.name_hash()) {
      case ros::hash("position"): deserialize(o.position, s); break;
      default: s.skip();
    }
  }
}

///////////////////////////////////////////////////////////////////
//hashing                                                        //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(World &o) {
    ros::hash_value h = ros::hash(o.position);
    return h;
  }
}
