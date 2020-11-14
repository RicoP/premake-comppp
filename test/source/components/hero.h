///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include "serializer.h"

#include "components/vector3.h"
#include "components/vector2.h"
#include "components/vector3.h"
#include "components/soundassetref.h"
#include "components/objectid.h"
#include "components/transform.h"
#include "components/vector3.h"
struct Hero {
  vector3 cam_offset;
  vector2 cam_speed;
  float heroRunSpeed;
  float heroWalkSpeed;
  vector3 inputv;
  bool jump;
  SoundAssetRef jump_sound;
  ObjectID renderObject;
  Transform transform;
  vector3 velocity;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(Hero));
    cam_offset = vector(0,0,1);
    cam_speed = vector(0.05f,0.05f);
    heroRunSpeed = 0.9f;
    heroWalkSpeed = 0.3f;
    inputv.setDefaultValues();
    jump_sound.setDefaultValues();
    renderObject.setDefaultValues();
    transform.setDefaultValues();
    velocity.setDefaultValues();
  }

  bool equals(const Hero & rhs) const {
    return
      cam_offset == rhs.cam_offset &&
      cam_speed == rhs.cam_speed &&
      heroRunSpeed == rhs.heroRunSpeed &&
      heroWalkSpeed == rhs.heroWalkSpeed &&
      inputv == rhs.inputv &&
      jump == rhs.jump &&
      jump_sound == rhs.jump_sound &&
      renderObject == rhs.renderObject &&
      transform == rhs.transform &&
      velocity == rhs.velocity;
  }
};

inline bool operator==(const Hero &lhs, const Hero &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const Hero &lhs, const Hero &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(Hero &o, ISerializer &s) {
  if(s.node_begin("Hero", ros::hash("Hero"), &o)) {
    s.key("cam_offset");
    serialize(o.cam_offset, s);
    s.key("cam_speed");
    serialize(o.cam_speed, s);
    s.key("heroRunSpeed");
    serialize(o.heroRunSpeed, s);
    s.key("heroWalkSpeed");
    serialize(o.heroWalkSpeed, s);
    s.key("inputv");
    serialize(o.inputv, s);
    s.key("jump");
    serialize(o.jump, s);
    s.key("jump_sound");
    serialize(o.jump_sound, s);
    s.key("renderObject");
    serialize(o.renderObject, s);
    s.key("transform");
    serialize(o.transform, s);
    s.key("velocity");
    serialize(o.velocity, s);
    s.node_end();
  }
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(Hero &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("cam_offset"):
        deserialize(o.cam_offset, s);
        break;
      case ros::hash("cam_speed"):
        deserialize(o.cam_speed, s);
        break;
      case ros::hash("heroRunSpeed"):
        deserialize(o.heroRunSpeed, s);
        break;
      case ros::hash("heroWalkSpeed"):
        deserialize(o.heroWalkSpeed, s);
        break;
      case ros::hash("inputv"):
        deserialize(o.inputv, s);
        break;
      case ros::hash("jump"):
        deserialize(o.jump, s);
        break;
      case ros::hash("jump_sound"):
        deserialize(o.jump_sound, s);
        break;
      case ros::hash("renderObject"):
        deserialize(o.renderObject, s);
        break;
      case ros::hash("transform"):
        deserialize(o.transform, s);
        break;
      case ros::hash("velocity"):
        deserialize(o.velocity, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(Hero &o) {
    ros::hash_value h = ros::hash(o.cam_offset);
    h = ros::xor64(h);
    h ^= ros::hash(o.cam_speed);
    h = ros::xor64(h);
    h ^= ros::hash(o.heroRunSpeed);
    h = ros::xor64(h);
    h ^= ros::hash(o.heroWalkSpeed);
    h = ros::xor64(h);
    h ^= ros::hash(o.inputv);
    h = ros::xor64(h);
    h ^= ros::hash(o.jump);
    h = ros::xor64(h);
    h ^= ros::hash(o.jump_sound);
    h = ros::xor64(h);
    h ^= ros::hash(o.renderObject);
    h = ros::xor64(h);
    h ^= ros::hash(o.transform);
    h = ros::xor64(h);
    h ^= ros::hash(o.velocity);
    return h;
  }
}

///////////////////////////////////////////////////////////////////
// randomize                                                     //
///////////////////////////////////////////////////////////////////
inline void randomize(Hero &o, ros::hash_value & h) {
  randomize(o.cam_offset, h);
  randomize(o.cam_speed, h);
  randomize(o.heroRunSpeed, h);
  randomize(o.heroWalkSpeed, h);
  randomize(o.inputv, h);
  randomize(o.jump, h);
  randomize(o.jump_sound, h);
  randomize(o.renderObject, h);
  randomize(o.transform, h);
  randomize(o.velocity, h);
}
