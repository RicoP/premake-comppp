///////////////////////////////////////////////////////////////////
// AUTOGENERATED do not edit by hand                             //
///////////////////////////////////////////////////////////////////
#pragma once
#include <serializer/serializer.h>

#include "components/camera.h"
#include "components/hero.h"
#include "components/sceneobject.h"
#include "components/skybox.h"
struct Scene {
  int activeCamera;
  rose::vectorPOD<16, Camera> cameras;
  Hero hero;
  rose::vectorPOD<1024, SceneObject> objects;
  Skybox skybox;
  ros::string<256> title;

  void setDefaultValues() {
    std::memset(this, 0, sizeof(Scene));
    activeCamera = 1;
    cameras.size = 0;
    hero.setDefaultValues();
    objects.size = 0;
    skybox.setDefaultValues();
  }

  bool equals(const Scene & rhs) const {
    return
      activeCamera == rhs.activeCamera &&
      cameras == rhs.cameras &&
      hero == rhs.hero &&
      objects == rhs.objects &&
      skybox == rhs.skybox &&
      title == rhs.title;
  }
};

inline bool operator==(const Scene &lhs, const Scene &rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(const Scene &lhs, const Scene &rhs) {
  return !lhs.equals(rhs);
}

///////////////////////////////////////////////////////////////////
// serializer                                                    //
///////////////////////////////////////////////////////////////////
inline void serialize(Scene &o, ISerializer &s) {
  if(s.node_begin("Scene", ros::hash("Scene"), &o)) {
    s.key("activeCamera");
    serialize(o.activeCamera, s);
    s.key("cameras");
    serialize(o.cameras, s);
    s.key("hero");
    serialize(o.hero, s);
    s.key("objects");
    serialize(o.objects, s);
    s.key("skybox");
    serialize(o.skybox, s);
    s.key("title");
    serialize(o.title, s);
    s.node_end();
  }
  s.end();
}

///////////////////////////////////////////////////////////////////
// deserializer                                                  //
///////////////////////////////////////////////////////////////////
inline void deserialize(Scene &o, IDeserializer &s) {
  o.setDefaultValues();
  while (s.next_key()) {
    switch (s.hash_key()) {
      case ros::hash("activeCamera"):
        deserialize(o.activeCamera, s);
        break;
      case ros::hash("cameras"):
        deserialize(o.cameras, s);
        break;
      case ros::hash("hero"):
        deserialize(o.hero, s);
        break;
      case ros::hash("objects"):
        deserialize(o.objects, s);
        break;
      case ros::hash("skybox"):
        deserialize(o.skybox, s);
        break;
      case ros::hash("title"):
        deserialize(o.title, s);
        break;
      default: s.skip_key(); break;
    }
  }
}

///////////////////////////////////////////////////////////////////
// hashing                                                       //
///////////////////////////////////////////////////////////////////
namespace ros {
  inline ros::hash_value hash(Scene &o) {
    ros::hash_value h = ros::hash(o.activeCamera);
    h = ros::xor64(h);
    h ^= ros::hash(o.cameras);
    h = ros::xor64(h);
    h ^= ros::hash(o.hero);
    h = ros::xor64(h);
    h ^= ros::hash(o.objects);
    h = ros::xor64(h);
    h ^= ros::hash(o.skybox);
    h = ros::xor64(h);
    h ^= ros::hash(o.title);
    return h;
  }
}

///////////////////////////////////////////////////////////////////
// randomize                                                     //
///////////////////////////////////////////////////////////////////
inline void randomize(Scene &o, ros::hash_value & h) {
  randomize(o.activeCamera, h);
  randomize(o.cameras, h);
  randomize(o.hero, h);
  randomize(o.objects, h);
  randomize(o.skybox, h);
  randomize(o.title, h);
}
