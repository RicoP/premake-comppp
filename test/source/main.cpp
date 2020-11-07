#include <cassert>

#include "components/vector3.h"
#include "components/player.h"
#include "components/world.h"

#include "jsonserializer.h"

int main() {
  JsonSerializer jsons;
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
  w.player.size = 4;

  w.scores.values[0] = 1;
  w.scores.values[1] = 1.3f;
  w.scores.values[2] = 42;
  w.scores.values[3] = 3.14f;
  w.scores.size = 4;

  serialize(w, jsons);

#define TEST(STR, VAL)           \
  do {                           \
    puts("serialize :");         \
    puts(STR);                   \
    puts("\n");                  \
    JsonDeserializer jsond(STR); \
    decltype(VAL) i;             \
    deserialize(i, jsond);       \
    assert(i == VAL);            \
  } while (0)

  TEST("123", 123);
  TEST("123,", 123);
  TEST("123.5", 123.5f);
  TEST(" 123.5", 123.5f);
  TEST(" 123.5 ", 123.5f);
  TEST("123.5 ", 123.5f);
  TEST("-123.0", -123.0f);
  TEST("-123.5,,,,,,", -123.5f);
  TEST("1e3,,,,,,", 1000.0f);

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

  char* doc = R"({
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
              "state" : "Active"
            }],
          "scores" : [1, 1.3, 42, 3.14]
        })";

  TEST(doc, w);

  char* doc2 =
      R"({"player":[{"position":{"x":1,"y":2,"z":3},"state":"Active"},{"position":{"x":10,"y":11,"z":12},"state":"Active"},{"position":{"x":1,"y":2,"z":3},"state":"Active"},
                  {"position":{"x":10,"y":11,"z":12},"state":"Active"}],"scores":[1,1.3,42,3.14]})";

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
          "state" : "Active"
        }
      ],
      "scores": [
        1,
        1.3,
        42,
        3.14
      ]
    })";

  TEST(doc3, w);

  return 0;
}