#include <cassert>

#include "components/vector2.h"
#include "components/vector3.h"
#include "components/quaternion.h"
#include "components/matrix4.h"

inline vector2 vector(float x, float y) {
  vector2 v = {x, y};
  return v;
}

inline vector3 vector(float x, float y, float z) {
  vector3 v = {x, y, z};
  return v;
}

inline matrix4 identity4() {
    matrix4 m;
    std::memset(&m, 0, sizeof(matrix4));
    m.m00 = 1;
    m.m11 = 1;
    m.m22 = 1;
    m.m33 = 1;
    return m;
}

inline quaternion qidentity() {
  quaternion q;
  q.x = 0;
  q.y = 0;
  q.z = 0;
  q.w = 1;
  return q;
}

#include "components/objectid.h"

inline ObjectID idgen() {
  static int id = 1;
  ObjectID o = {id++};
  return o;
}

#include "components/soundassetref.h"
#include "components/meshassetref.h"
#include "components/textureassetref.h"
#include "components/shaderassetref.h"
#include "components/collider.h"

#include "components/player.h"
#include "components/world.h"

#include "components/transform.h"
#include "components/sceneobject.h"
#include "components/camera.h"
#include "components/skybox.h"
#include "components/collider.h"
#include "components/sceneObject.h"
#include "components/hero.h"
#include "components/scene.h"

#include "jsonserializer.h"

int main() {
  FILE* f = fopen("foo.json", "w");
  JsonSerializer jsons(f);
  vector3 v = {1, 2, 3};

  Player p1;
  p1.position = v;
  p1.state = Player::State::Active;

  Player p2;
  p2.position = {10, 11, 12};
  p2.state = Player::State::Active;

  World w;
  w.setDefaultValues();
  w.player.values[0] = p1;
  w.player.values[1] = p2;
  w.player.values[2] = p1;
  w.player.values[3] = p2;
  w.player.values[3].state = Player::State::Active | Player::State::Jumping;
  w.player.size = 4;

  w.scores.values[0] = 1;
  w.scores.values[1] = 1.3f;
  w.scores.values[2] = 42;
  w.scores.values[3] = 3.14f;
  w.scores.size = 4;

  strcpy(w.title.data, "Hello");

  serialize(w, jsons);
  fclose(f);

  // Test serializer in out for World
  {
    char* buffer = 0;
    long length;
    FILE* f = fopen("foo.json", "rb");

    assert(f);

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = (char*)std::malloc(length);
    if (buffer) {
      fread(buffer, 1, length, f);
      buffer[length] = 0;
    }
    fclose(f);

    World w2;
    JsonDeserializer jsond(buffer);
    deserialize(w2, jsond);

    assert(w == w2);

    std::free(buffer);
    buffer = 0;
    puts("");
  }

  // Test serializer in out for Scene
  {
    Scene s1;
    s1.setDefaultValues();
    s1.objects.size = 1;
    auto & o = s1.objects.values[0];
    o.setDefaultValues();

    FILE* f = fopen("scene.json", "w");
    JsonSerializer jsons(f);

    serialize(s1, jsons);
    fclose(f);

    char* buffer = 0;
    long length;
    f = fopen("scene.json", "rb");

    assert(f);

    fseek(f, 0, SEEK_END);
    length = ftell(f)+1;
    fseek(f, 0, SEEK_SET);
    buffer = (char*)std::malloc(length);
    if (buffer) {
      fread(buffer, 1, length-1, f);
      buffer[length-1] = 0;
    }
    fclose(f);

    Scene s2;
    JsonDeserializer jsond(buffer);
    deserialize(s2, jsond);

    assert(s1 == s2);

    std::free(buffer);
    buffer = 0;
    puts("");
  }

#define TEST(STR, VAL)           \
  do {                           \
    puts("serialize :");         \
    puts(STR);                   \
    JsonDeserializer jsond(STR); \
    decltype(VAL) i;             \
    deserialize(i, jsond);       \
    assert(i == (VAL));          \
    puts("OK\n");                \
  } while (0)

  TEST("123", 123);
  TEST("123,", 123);
  TEST("123.5", 123.5f);
  TEST(" 123.5", 123.5f);
  TEST(" 123.5 ", 123.5f);
  TEST("123.5 ", 123.5f);
  TEST("-123.0", -123.0f);
  TEST("-123.5,", -123.5f);
  TEST("1e3,", 1000.0f);

  ros::array<4, int> vec;
  vec.size = 4;
  vec.values[0] = 1;
  vec.values[1] = 2;
  vec.values[2] = 3;
  vec.values[3] = 4;

  TEST(R"([1,2,3,4])", vec);
  TEST(R"([ 1, 2, 3, 4])", vec);
  TEST(R"([1 ,2 ,3 ,4 ])", vec);
  TEST(R"([ 1 , 2 ,   3 ,   4 ])", vec);
  TEST("[ 1 , 2 ,   3 \n,   4 ]", vec);
  TEST("[ 1 , 2 ,   3 ,\n   4 ]", vec);
  TEST("[ 1 , 2 ,   3 ,\r\n   4 ]", vec);
  TEST("[ 1 , 2 ,   3 \r\n,\r\n   4 ]", vec);

  ros::array<4, float> fvec;
  fvec.size = 4;
  fvec.values[0] = 1;
  fvec.values[1] = 2;
  fvec.values[2] = 3;
  fvec.values[3] = 4;
  TEST(R"([ 1 , 2 ,    3.0 ,   4 ])", fvec);
  TEST(R"([ 1    , 2 ,   3 ,   4 ])", fvec);
  TEST(R"([ 1 , 2 ,   3.0 ,    4 ])", fvec);
  TEST(R"([ 1. , 2 ,   3.0 , 4   ])", fvec);
  fvec.values[0] = 1000;
  fvec.values[1] = -2;
  fvec.values[2] = 1500;
  fvec.values[3] = 0;
  TEST(R"([1e3, -2e0, 1.5e3, 0e10])", fvec);

  TEST(R"({"x" : 1, "y" : 2, "z" : 3})", v);

  TEST(R"("Active")", Player::State::Active);
  TEST(R"("Active|Jumping")", Player::State::Active | Player::State::Jumping);

  TEST(R"("   Active")", Player::State::Active);
  TEST(R"("   Active     ")", Player::State::Active);

  TEST(R"("   Active|Jumping")", Player::State::Active | Player::State::Jumping);
  TEST(R"("   Active|Jumping    ")", Player::State::Active | Player::State::Jumping);
  TEST(R"("   Active   |    Jumping    ")", Player::State::Active | Player::State::Jumping);
  TEST(R"("   Active   |Jumping    ")", Player::State::Active | Player::State::Jumping);

  char* doc = R"(
        {
          "player" : [{
              "position" : {
                "x" : 1,
                "y" : 2,
                "z" : 3
              },
              "state" : "Active"
            }, {
              "position" : {
                "x" : 10,
                "y" : 11,
                "z" : 12
              },
              "state" : "Active"
            }, {
              "position" : {
                "x" : 1,
                "y" : 2,
                "z" : 3
              },
              "state" : "Active"
            }, {
              "position" : {
                "x" : 10,
                "y" : 11,
                "z" : 12
              },
              "state" : "Active|Jumping"
            }],
          "scores" : [1, 1.3, 42, 3.14],
          "title" : "Hello"
        })";

  TEST(doc, w);

  char* doc2 =
      R"({"player":[{"position":{"x":1,"y":2,"z":3},"state":"Active"},{"position":{"x":10,"y":11,"z":12},"state":"Active"},{"position":{"x":1,"y":2,"z":3},"state":"Active"},
                  {"position":{"x":10,"y":11,"z":12},"state":"Active|Jumping"}],"scores":[1,1.3,42,3.14],"title":"Hello"})";

  TEST(doc2, w);

  char* doc3 = R"(
    {
      "player": [
        {
          "position": {
            "x": 1,
            "y": 2,
            "z": 3
          },
          "state" : "Active"
        },
        {
          "position": {
            "x": 10,
            "y": 11,
            "z": 12
          },
          "state" : "Active"
        },
        {
          "position": {
            "x": 1,
            "y": 2,
            "z": 3
          },
          "state" : "Active"
        },
        {
          "position": {
            "x": 10,
            "y": 11,
            "z": 12
          },
          "state" : "Active|Jumping"
        }
      ],
      "title" : "Hello",
      "scores": [
        1,
        1.3,
        42,
        3.14
      ]
    })";

  TEST(doc3, w);

  puts("_______");
  puts("All OK!");
  return 0;
}