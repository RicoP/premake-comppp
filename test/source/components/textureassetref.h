///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include "serializer.h"

struct TextureAssetRef {
  int id;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(TextureAssetRef));
  }

  bool equals(const TextureAssetRef & rhs) const {
    return
      id == rhs.id;
  }
};

inline bool operator==(const TextureAssetRef &lhs, const TextureAssetRef &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const TextureAssetRef &lhs, const TextureAssetRef &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(TextureAssetRef &o, ISerializer &s) {
  s.begin();
  s.key("id");
  serialize(o.id, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(TextureAssetRef &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("id"):
        deserialize(o.id, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(TextureAssetRef &o) {
    ros::hash_value h = ros::hash(o.id);
    return h;
  }
}

///////////////////////////////////////////////////////////////////
// randomize                                                     //
///////////////////////////////////////////////////////////////////
inline void randomize(TextureAssetRef &o, ros::hash_value & h) {
  randomize(o.id, h);
}