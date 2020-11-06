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

  void print_indent() { std::cout << '\n' << (whitespace  + 128 - indent_depth*2); }

  virtual void set_field_name(const char* name) override { queue_char(',');             print_indent(); std::cout << "\"" << name << "\" : "; queued[indent_depth] = 0; _name = name; }
  virtual void hint_type(const char* type)      override { queue_char(',');   indent();                 std::cout << "{"; }
  virtual void end()                            override {                  unindent(); print_indent(); std::cout << "}"; }
  virtual void begin_array()                    override { queue_char(',');   indent();                 std::cout << "["; }
  virtual void end_array()                      override {                  unindent();                 std::cout << "]"; }
  virtual void do_float(float f)                override { queue_char(',');                             std::cout << f;   }
  virtual void do_int(int i)                    override { queue_char(',');                             std::cout << i;   }
};

struct JsonDeserializer : public IDeserializer {
  char* _json;
  char* _p;

  JsonDeserializer(char* json) {
    _json = json;
    _p = _json;
  }

  bool is_whitespace(char c) {
    switch (c) {
      case ' ':
      case '\t':
      case '\r':
      case '\n':
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

  void expect(char c) {
    trim();
    if (*_p != c)
      std::cerr << "Error: expected " << c << " got " << *_p << ".\n";
    else
      ++_p;
  }

  virtual bool next() override { return true; }
  virtual void skip() override {}
  virtual ros::hash_value name_hash() override { return 0; }

  virtual void begin_array() override { expect('['); }

  virtual bool in_array() override {
    char c = current();
    if (c == ']') return false;
    if (c == ',') ++_p;
    // std::cerr << "Error: either , or ] but got " << c << ".\n";
    // return false;
    return true;
  }

  virtual void do_int(int& i) override { i = strtol(_p, &_p, 10); }
  virtual void do_float(float& f) override { f = (float)strtod(_p, &_p); }
};

int main() {
  JsonSerializer jsons;
  vector3 v = {1, 2, 3};

  Player p1;
  p1.position = v;
  p1.state = 1337;

  Player p2;
  p2.position = {.x = 10, .y = 11, .z = 12};
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

#define TEST(STR, VAL)                         \
  do {                                         \
    std::cout << "serialize :" << STR << "\n"; \
    JsonDeserializer jsond(STR);               \
    decltype(VAL) i;                           \
    deserialize(i, jsond);                     \
    assert(i == VAL);                          \
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

  // TEST(R"({"x" : 1, "y" : 2, "z" : 3})", v);

  return 0;
}