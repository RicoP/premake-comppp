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

Player::State operator|(Player::State lhs, Player::State rhs) {
  return static_cast<Player::State>(static_cast<char>(lhs) | static_cast<char>(rhs));
}

Player::State operator|=(Player::State & lhs, Player::State rhs) {
  return lhs = lhs | rhs;
}

Player::State operator&(Player::State lhs, Player::State rhs) {
  return static_cast<Player::State>(static_cast<char>(lhs) & static_cast<char>(rhs));
}

Player::State operator&=(Player::State & lhs, Player::State rhs) {
  return lhs = lhs & rhs;
}

inline void serialize(Player::State &o, ISerializer &s) {
  if ((o & Player::State::Active) != Player::State::None) s.write_enum("Active");
  if ((o & Player::State::Jumping) != Player::State::None) s.write_enum("Jumping");
  s.end_enum();
}

inline void deserialize(Player::State &o, IDeserializer &d) {
  o = Player::State::None;
  while (d.in_enum()) {
    switch (d.hash_key()) {
      case ros::hash("Active"):
        o |= Player::State::Active;
        break;
      case ros::hash("Jumping"):
        o |= Player::State::Jumping;
        break;
      default:
        break;
    }
  }
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
  s.begin();
  s.key("position");
  serialize(o.position, s);
  s.key("state");
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
