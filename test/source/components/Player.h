#pragma once

#include "serializer.h"

struct Player {
  enum class State : long long {
    None = 0,
    Active = 1 << 0,
    Jumping = 1 << 1,
  };

  vector3 position;
  State state;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(Player));
    position.setDefaultValues();
  }

  bool equals(const Player &rhs) const {
    return position == rhs.position && state == rhs.state;
  }
};

inline void serialize(Player::State &o, ISerializer &s) {
  long long l = (long long)o;
  // TODO: switch case for all enum values;
  serialize(l, s);
}

inline void deserialize(Player::State &o, IDeserializer &d) {
  long long l = 0;
  d.begin_enum();
  while (d.in_enum()) {
    switch (d.hash_key()) {
      case ros::hash("Active"):
        l |= (long long)Player::State::Active;
        break;
      case ros::hash("Jumping"):
        l |= (long long)Player::State::Jumping;
        break;
      default:
        break;
    }
  }
  o = (Player::State)l;
}

bool operator==(const Player &lhs, const Player &rhs) {
  return lhs.equals(rhs);
}
bool operator!=(const Player &lhs, const Player &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
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
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(Player &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("position"):
        deserialize(o.position, s);
        break;
      case ros::hash("state"):
        deserialize(o.state, s);
        break;
      default:
        s.skip_key();
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(Player &o) {
    ros::hash_value h = ros::hash(o.position);
    h = ros::xor64(h);
    h ^= ros::hash((long long)o.state);
    return h;
  }
}  // namespace ros
