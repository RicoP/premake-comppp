#include <iostream>

#include "components/vector3.h"
#include "components/Player.h"
#include "components/World.h"

struct JsonSerializer : public ISerializer {
  const char* _name = "";
  char queued[64] = {0};
  int indent_depth = 0;

  void queue_char(char c) {
    if (queued[indent_depth]) std::cout << queued[indent_depth] << " ";
    queued[indent_depth] = c;
  }

  void indent() { ++indent_depth; }
  void unindent() { queued[indent_depth--] = 0; }

  void print_indent() { std::cout << '\n' << std::string(indent_depth*2, ' '); }

  virtual void set_field_name(const char* name) override { queue_char(',');             print_indent(); std::cout << "\"" << name << "\" : "; queued[indent_depth] = 0; _name = name; }
  virtual void hint_type(const char* type)      override { queue_char(',');   indent();                 std::cout << "{"; }
  virtual void end()                            override {                  unindent(); print_indent(); std::cout << "}"; }
  virtual void begin_array()                    override { queue_char(',');   indent();                 std::cout << "["; }
  virtual void end_array()                      override {                  unindent();                 std::cout << "]"; }
  virtual void do_float(float f)                override { queue_char(',');                             std::cout << f;   }
  virtual void do_int(int i)                    override { queue_char(',');                             std::cout << i;   }
};

// struct JsonDeserializer : public IDeserializer {};

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

  return 0;
}