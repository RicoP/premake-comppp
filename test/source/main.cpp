#include <cassert>
#include <iostream>

#include "components/vector3.h"
#include "components/Player.h"
#include "components/World.h"

struct JsonSerializer : public ISerializer {
  const char* _name = "";
  char queued[64] = {0};
  char whitespace[128 + 1] = "                                                                                                                                ";
  int indent_depth = 0;

  void queue_char(char c) {
    if (queued[indent_depth]) std::cout << queued[indent_depth] << " ";
    queued[indent_depth] = c;
  }

  void indent() { ++indent_depth; }
  void unindent() { queued[indent_depth--] = 0; }

  void print_indent() {
    std::cout << '\n' << (whitespace + 128 - indent_depth * 2);
  }

  virtual void set_field_name(const char* name) override { queue_char(',');             print_indent(); std::cout << "\"" << name << "\" : "; queued[indent_depth] = 0; _name = name; }
  virtual void hint_type(const char* type)      override { queue_char(',');   indent();                 std::cout << "{"; }
  virtual void end()                            override {                  unindent(); print_indent(); std::cout << "}"; }
  virtual void begin_array()                    override { queue_char(',');   indent();                 std::cout << "["; }
  virtual void end_array()                      override {                  unindent();                 std::cout << "]"; }
  virtual void do_float(float f)                override { queue_char(',');                             std::cout << f;   }
  virtual void do_int(int i)                    override { queue_char(',');                             std::cout << i;   }
};

struct JsonDeserializer : public IDeserializer {
  char* _json = 0;
  char* _p = 0;
  ros::hash_value _name_hash = 0;

  JsonDeserializer(char* json) {
    _json = json;
    _p = _json;
  }

  bool is_whitespace(char c) {
    switch (c) {
      case ' ': case '\t': case '\r': case '\n':
      case ',': case ':': // we consider ',' and ':' a whitespace
      case '{': case '[':
        return true;
      default:
        return false;
    }
  }

  void trim() {
    while (is_whitespace(*_p)) ++_p;
  }

  char current() {
    trim();
    return *_p;
  }

  char* seek(char c) {
    char* p = _p;
    while (*p != c && *p != 0) ++p;
    if (*p == 0) std::cerr << "Expected " << c << " but found EOF.";
    return p;
  }

  void expect(char c) {
    trim();
    if (*_p != c)
      std::cerr << "Error: expected " << c << " got " << *_p << ".\n";
    else
      ++_p;
  }

  virtual ros::hash_value hash_key() override { return _name_hash; }

  virtual bool next_key() override {
    char c = current();
    if (c == '}') {
      expect('}');  // document end
      return false;
    }

    expect('\"');
    char* end = seek('\"');
    _name_hash = ros::hash_fnv(_p, end);
    if (*end != 0) {
      _p = end + 1;
      return true;
    }
    return false;
  }

  virtual void skip_key() override { /*TODO*/ }

  virtual bool in_array() override {
    char c = current();
    if (c == ']') { expect(']'); return false; }
    return true;
  }

  virtual void do_int(int& i) override {
    trim();
    i = (int)strtol(_p, &_p, 10);
  }

  virtual void do_float(float& f) override {
    trim();
    f = (float)strtod(_p, &_p);
  }
};

int main() {
  JsonSerializer jsons;
  vector3 v = {1, 2, 3};

  Player p1;
  p1.position = v;
  p1.state = 1337;

  Player p2;
  p2.position = {10, 11,12};
  p2.state = 1337;

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

#define TEST(STR, VAL)                          \
  do {                                          \
    puts("serialize :"); puts(STR); puts("\n"); \
    JsonDeserializer jsond(STR);                \
    decltype(VAL) i;                            \
    deserialize(i, jsond);                      \
    assert(i == VAL);                           \
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

  char* doc = R"({
          "player" : [{
              "position" : {
                "x" : 1,
                "y" : 2,
                "z" : 3
              },
              "state" : 1337
            }, {
              "position" : {
                "x" : 10,
                "y" : 11,
                "z" : 12
              },
              "state" : 1337
            }, {
              "position" : {
                "x" : 1,
                "y" : 2,
                "z" : 3
              },
              "state" : 1337
            }, {
              "position" : {
                "x" : 10,
                "y" : 11,
                "z" : 12
              },
              "state" : 1337
            }],
          "scores" : [1, 1.3, 42, 3.14]
        })";

  TEST(doc, w);

  char* doc2 =
      R"({"player":[{"position":{"x":1,"y":2,"z":3},"state":1337},{"position":{"x":10,"y":11,"z":12},"state":1337},{"position":{"x":1,"y":2,"z":3},"state":1337},
                  {"position":{"x":10,"y":11,"z":12},"state":1337}],"scores":[1,1.3,42,3.14]})";

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
          "state": 1337
        },
        {
          "position": {
            "x": 10,
            "y": 11,
            "z": 12
          },
          "state": 1337
        },
        {
          "position": {
            "x": 1,
            "y": 2,
            "z": 3
          },
          "state": 1337
        },
        {
          "position": {
            "x": 10,
            "y": 11,
            "z": 12
          },
          "state": 1337
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