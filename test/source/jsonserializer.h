#include <iostream>

#include "serializer.h"

struct JsonSerializer : public ISerializer {
  enum { MAX_DEPTH = 64 };
  char queued[MAX_DEPTH] = {0};
  char whitespace[MAX_DEPTH * 2 + 1] = {0};
  int indent_depth = 0;

  JsonSerializer() { std::memset(whitespace, ' ', MAX_DEPTH * 2); }

  template <class T, class... Ts>
  void put(T a, Ts... bcd)  { put(a); put(bcd...);                            }
  void put(const char* str) { printf("%s", str);                              }
  void put(      char* str) { printf("%s", str);                              }
  void put(        float f) { printf("%g", f);                                }
  void put(         char c) { printf("%c", c);                                }
  void put(          int i) { printf("%d", i);                                }
  void print_indent()       { put("\n", whitespace + 128 - indent_depth * 2); }

  char queue()              { return queued[indent_depth];                    }
  void clear()              { queued[indent_depth] = 0;                       }
  void indent()             { ++indent_depth;                                 }
  void unindent()           { queued[indent_depth--] = 0;                     }

  void queue_char(char c) {
    if (queued[indent_depth]) put(queued[indent_depth], c==','?" ":"");
    queued[indent_depth] = c;
  }

  virtual void key(const char* name)     override { queue_char(',');             print_indent();  put("\"", name, "\" : "); clear(); }
  virtual void begin()                   override { queue_char(',');   indent();                  put("{");                          }
  virtual void end()                     override {                  unindent(); print_indent();  put("}");                          }
  virtual void begin_array()             override { queue_char(',');   indent();                  put("[");                          }
  virtual void end_array()               override {                  unindent();                  put("]");                          }
  virtual void write_enum(const char* e) override { if(queue() == 0) put("\"");  queue_char('|'); put(e);                            }
  virtual void end_enum()                override {                                               put("\"");                clear(); }
  virtual void do_float(float f)         override { queue_char(',');                              put(f);                            }
  virtual void do_int(int i)             override { queue_char(',');                              put(i);                            }
};

struct JsonDeserializer : public IDeserializer {
  char* _p = 0;
  hash _name_hash = 0;
  bool enum_started = false;

  JsonDeserializer(char* json) {
    _p = json;
  }

  bool is_whitespace(char c) {
    switch (c) {
      case ' ': case '\t': case '\r': case '\n':
      case ',': case ':': // we consider ',' and ':' a whitespace
      case '{': case '[': // we also consider beginning tokens a whitespace
      case '|':
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

  bool contains(char c, const char* chars) {
    while (*chars) {
      if (*chars == c) return true;
      chars++;
    }
    return false;
  }

  char* seek(const char* chars) {
    char* p = _p;
    while (*p != 0 && !contains(*p, chars)) ++p;
    if (*p == 0)
      std::cerr << "Error: expected '" << chars << "' but got "
                << "EOF"
                << ".\n";
    return p;
  }

  char* seek(char c) {
    char* p = _p;
    while (*p != c && *p != 0) ++p;
    if (*p == 0)
      std::cerr << "Error: expected '" << c << "' but got "
                << "EOF"
                << ".\n";
    return p;
  }

  void expect(char c) {
    trim();
    if (*_p != c)
      std::cerr << "Error: expected '" << c << "' but got " << *_p << ".\n";
    else
      ++_p;
  }

  virtual hash hash_key() override { return _name_hash; }

  virtual bool next_key() override {
    char c = current();
    if (c == '}') {
      expect('}');  // document end
      return false;
    }

    expect('\"');
    char* end = seek('\"');
    if (*end != 0) {
      _name_hash = ros::hash_fnv(_p, end);
      _p = end + 1;
      return true;
    }
    return false;
  }

  virtual bool in_enum() override {
    if(!enum_started) { expect('\"'); enum_started = true; }
    trim();
    //_p points on first character of next enum or on "
    if (current() == '\"') {
      _p++;
      enum_started = false;
      return false;
    }
    if (current() == '\n') {
      std::cerr << "Error: expected '" << "String" << "' but got " << "EOL" << ".\n";
      return false;
    }

    char* end = seek(" \t|\"\n");
    _name_hash = ros::hash_fnv(_p, end);
    _p = end;
    return true;
  }

  virtual void skip_key() override { /*TODO*/ }

  virtual bool in_array() override {
    char c = current();
    if (c == ']') {
      expect(']');
      return false;
    }
    return true;
  }

  virtual void do_float(float& f) override {
    trim();
    f = strtof(_p, &_p);
  }

  virtual void do_int(int& i) override {
    trim();
    i = (int)strtol(_p, &_p, 10);
  }

  /*virtual void do_long(long long& i) override {
    trim();
    i = strtoll(_p, &_p, 10);
  }*/
};