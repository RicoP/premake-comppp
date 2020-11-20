///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include <serializer/serializer.h>

struct ObjectID {
  int id;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(ObjectID));
  }

  bool equals(const ObjectID & rhs) const {
    return
      id == rhs.id;
  }
};

inline bool operator==(const ObjectID &lhs, const ObjectID &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const ObjectID &lhs, const ObjectID &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(ObjectID &o, ISerializer &s) {
  if(s.node_begin("ObjectID", ros::hash("ObjectID"), &o)) {
    s.key("id");
    serialize(o.id, s);
    s.node_end();
  }
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(ObjectID &o, IDeserializer &s) {
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
  inline ros::hash_value hash(ObjectID &o) {
    ros::hash_value h = ros::hash(o.id);
    return h;
  }
}

///////////////////////////////////////////////////////////////////
// randomize                                                     //
///////////////////////////////////////////////////////////////////
inline void randomize(ObjectID &o, ros::hash_value & h) {
  randomize(o.id, h);
}
