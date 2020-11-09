///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include "serializer.h"

struct matrix4 {
  float m00;
  float m01;
  float m02;
  float m03;
  float m10;
  float m11;
  float m12;
  float m13;
  float m20;
  float m21;
  float m22;
  float m23;
  float m30;
  float m31;
  float m32;
  float m33;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(matrix4));
  }

  bool equals(const matrix4 & rhs) const {
    return
      m00 == rhs.m00 &&
      m01 == rhs.m01 &&
      m02 == rhs.m02 &&
      m03 == rhs.m03 &&
      m10 == rhs.m10 &&
      m11 == rhs.m11 &&
      m12 == rhs.m12 &&
      m13 == rhs.m13 &&
      m20 == rhs.m20 &&
      m21 == rhs.m21 &&
      m22 == rhs.m22 &&
      m23 == rhs.m23 &&
      m30 == rhs.m30 &&
      m31 == rhs.m31 &&
      m32 == rhs.m32 &&
      m33 == rhs.m33;
  }
};

inline bool operator==(const matrix4 &lhs, const matrix4 &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const matrix4 &lhs, const matrix4 &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(matrix4 &o, ISerializer &s) {
  s.begin();
  s.key("m00");
  serialize(o.m00, s);
  s.key("m01");
  serialize(o.m01, s);
  s.key("m02");
  serialize(o.m02, s);
  s.key("m03");
  serialize(o.m03, s);
  s.key("m10");
  serialize(o.m10, s);
  s.key("m11");
  serialize(o.m11, s);
  s.key("m12");
  serialize(o.m12, s);
  s.key("m13");
  serialize(o.m13, s);
  s.key("m20");
  serialize(o.m20, s);
  s.key("m21");
  serialize(o.m21, s);
  s.key("m22");
  serialize(o.m22, s);
  s.key("m23");
  serialize(o.m23, s);
  s.key("m30");
  serialize(o.m30, s);
  s.key("m31");
  serialize(o.m31, s);
  s.key("m32");
  serialize(o.m32, s);
  s.key("m33");
  serialize(o.m33, s);
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(matrix4 &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("m00"):
        deserialize(o.m00, s);
        break;
      case ros::hash("m01"):
        deserialize(o.m01, s);
        break;
      case ros::hash("m02"):
        deserialize(o.m02, s);
        break;
      case ros::hash("m03"):
        deserialize(o.m03, s);
        break;
      case ros::hash("m10"):
        deserialize(o.m10, s);
        break;
      case ros::hash("m11"):
        deserialize(o.m11, s);
        break;
      case ros::hash("m12"):
        deserialize(o.m12, s);
        break;
      case ros::hash("m13"):
        deserialize(o.m13, s);
        break;
      case ros::hash("m20"):
        deserialize(o.m20, s);
        break;
      case ros::hash("m21"):
        deserialize(o.m21, s);
        break;
      case ros::hash("m22"):
        deserialize(o.m22, s);
        break;
      case ros::hash("m23"):
        deserialize(o.m23, s);
        break;
      case ros::hash("m30"):
        deserialize(o.m30, s);
        break;
      case ros::hash("m31"):
        deserialize(o.m31, s);
        break;
      case ros::hash("m32"):
        deserialize(o.m32, s);
        break;
      case ros::hash("m33"):
        deserialize(o.m33, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(matrix4 &o) {
    ros::hash_value h = ros::hash(o.m00);
    h = ros::xor64(h);
    h ^= ros::hash(o.m01);
    h = ros::xor64(h);
    h ^= ros::hash(o.m02);
    h = ros::xor64(h);
    h ^= ros::hash(o.m03);
    h = ros::xor64(h);
    h ^= ros::hash(o.m10);
    h = ros::xor64(h);
    h ^= ros::hash(o.m11);
    h = ros::xor64(h);
    h ^= ros::hash(o.m12);
    h = ros::xor64(h);
    h ^= ros::hash(o.m13);
    h = ros::xor64(h);
    h ^= ros::hash(o.m20);
    h = ros::xor64(h);
    h ^= ros::hash(o.m21);
    h = ros::xor64(h);
    h ^= ros::hash(o.m22);
    h = ros::xor64(h);
    h ^= ros::hash(o.m23);
    h = ros::xor64(h);
    h ^= ros::hash(o.m30);
    h = ros::xor64(h);
    h ^= ros::hash(o.m31);
    h = ros::xor64(h);
    h ^= ros::hash(o.m32);
    h = ros::xor64(h);
    h ^= ros::hash(o.m33);
    return h;
  }
}
