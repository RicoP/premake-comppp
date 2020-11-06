#pragma once
#include "serializer.h"

struct World {
  ros::array<4, Player> player;
  ros::array<4, float> scores;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(World));
    player.size = 0;
    scores.size = 0;
  }

  bool equals(const World & rhs) const {
    return
      player == rhs.player &&
      scores == rhs.scores;
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
  s.set_field_name("player");
  serialize(o.player, s);
  s.set_field_name("scores");
  serialize(o.scores, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
//deserializer                                                   //
///////////////////////////////////////////////////////////////////
inline void deserialize(World &o, IDeserializer &s) {
  o.setDefaultValues();
  s.begin_document();
  while (s.next()) {
    switch (s.name_hash()) {
      case ros::hash("player"): deserialize(o.player, s); break;
      case ros::hash("scores"): deserialize(o.scores, s); break;
      default: s.skip();
    }
  }
}

///////////////////////////////////////////////////////////////////
//hashing                                                        //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(World &o) {
    ros::hash_value h = ros::hash(o.player);
    h = ros::xor64(h);
    h ^= ros::hash(o.scores);
    return h;
  }
}
