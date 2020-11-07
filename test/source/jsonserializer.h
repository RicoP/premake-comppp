#include <iostream>

#include "serializer.h"

struct JsonSerializer : public ISerializer {
  const char* _name = "";
  enum { MAX_DEPTH = 64 };
  char queued[MAX_DEPTH] = {0};
  char whitespace[MAX_DEPTH * 2 + 1] = {0};
  int indent_depth = 0;

  JsonSerializer() { std::memset(whitespace, ' ', MAX_DEPTH * 2); }

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
      case '{': case '[': // we also consider beginning tokens a whitespace
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

  virtual ros::hash_value hash_key() override { return _name_hash; }

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

  virtual void begin_enum() override { expect('\"'); }

  virtual bool in_enum() override {
    trim();
    //_p points on first character of next enum or on "
    if (current() == '\"') {
        _p++;
        return false;
    }
    if (current() == '\n') {
      std::cerr << "Error: expected '" << "String" << "' but got " << "EOL" << ".\n";
      return false;
    }

    char* end = seek(",\"\n");
    _name_hash = ros::hash_fnv(_p, end);
    _p = end;
    return true;
  }

  virtual void skip_key() override { /*TODO*/
  }

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
    f = (float)strtod(_p, &_p);
  }

  virtual void do_int(int& i) override {
    trim();
    i = (int)strtol(_p, &_p, 10);
  }

  virtual void do_long(long long& i) override {
    trim();
    i = strtoll(_p, &_p, 10);
  }
};
