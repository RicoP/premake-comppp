#pragma once

#include <iostream>
#include <cfloat>

#include "serializer.h"

struct JsonSerializer : public ISerializer {
  enum { MAX_DEPTH = 64 };
  char queued[MAX_DEPTH] = {0};
  char whitespace[MAX_DEPTH * 2 + 1] = {0};
  int indent_depth = 0;
  FILE* file = stdout;

  JsonSerializer() { std::memset(whitespace, ' ', MAX_DEPTH * 2); }
  JsonSerializer(FILE* file_) : file(file_) { std::memset(whitespace, ' ', MAX_DEPTH * 2); }

  template <class T, class... Ts>
  void put(T a, Ts... bcd)  { put(a); put(bcd...);                            }
  void put(const char* str) { fprintf(file, "%s", str);                       }
  void put(    long long l) { fprintf(file, "%lld", l);                       }
  void put(      char* str) { fprintf(file, "%s", str);                       }
  void put(         char c) { fputc(c, file);                                 }
  void put(          int i) { fprintf(file, "%d", i);                         }
  void put(unsigned long long l) { fprintf(file, "%llu", l);                  }
  void put(        float f) { 
    if(f == 0) {
      //Special case because float can be both +0 and -0
      fputc('0', file);
      return;
    }
    
    //Pretty Print float with double precission and cut of trailing zeros.
    char buffer[128];
    //https://stackoverflow.com/a/19897395
    sprintf(buffer, "%.*f", DBL_DECIMAL_DIG - 1, (double)f); 
    int last_non_zero_index = 0;
    int dot_index = 0;

    int i = 0;
    while(buffer[i]) {
      if(buffer[i] == '.') dot_index = i;
      if(buffer[i] != '0') last_non_zero_index = i;

      ++i;
    }

    if(dot_index > 0) {
      if(dot_index == last_non_zero_index) buffer[dot_index] = 0;
      else if(last_non_zero_index > dot_index) buffer[last_non_zero_index + 1] = 0;
    }

    fprintf(file, "%s", buffer); 
  }
  void print_indent()       { put("\n", whitespace + 128 - indent_depth * 2); }

  bool is_printable(char c) {
    return (c >= ' ' && c <= '~');
  }

  bool is_simple_printable(char c) {
    switch(c) {
	//This characters need special attention
    case '"': case '/': case '\\': return false;
    default: break;
    }
    return is_printable(c);
  }

  void print_zeros(size_t leading_zeros) {
    if (leading_zeros) {
      for (size_t i = leading_zeros; i != 0; --i) {
        put("\\u0000");
      }
    }
  }

  void decode_string(const char * s, const char * end) {
      size_t leading_zeros = 0;
      for(;s != end; ++s) {
        char c = *s;

        if(c == 0) { leading_zeros++; continue; }

        print_zeros(leading_zeros);
        leading_zeros = 0;

        if(is_simple_printable(c)) put(c);
        else {
            switch(c) {
                case '\n': put("\\n"); break;
                case '\r': put("\\r"); break;
                case '\\': put("\\\\"); break;
                case '/':  put("\\/"); break;
                case '\b': put("\\b"); break;
                case '\f': put("\\f"); break;
                case '\t': put("\\t"); break;
                case '\"': put("\\\""); break;
                default:
                    fprintf(file, "\\u00%02X", (unsigned)c); break;
            }
        }
      }
  }

  char queue()              { return queued[indent_depth];                    }
  void clear()              { queued[indent_depth] = 0;                       }
  void indent()             { ++indent_depth;                                 }
  void unindent()           { queued[indent_depth--] = 0;                     }

  void queue_char(char c) {
    if (queued[indent_depth]) {
        put(queued[indent_depth]);
        if(c==',') put(" ");
    }
    queued[indent_depth] = c;
  }

  virtual bool node_begin(const char *, RHash, void *) override { queue_char(',');      indent(); put("{");                return true; }

  virtual void key(const char* name)     override { queue_char(',');             print_indent();  put("\"", name, "\" : "); clear(); }
  virtual bool begin_array()             override { queue_char(',');   indent();                  put("[");                          return true; }
  virtual void in_array(size_t)          override { }
  virtual void end_array()               override {                  unindent();                  put("]");                          }
  virtual void write_enum(const char* e) override { if(queue() == 0) put("\"");  queue_char('|'); put(e);                            }
  virtual void end_enum()                override {                                               put("\"");                clear(); }

  virtual void do_string(char* begin, char* end) override { queue_char(','); put("\""); decode_string(begin, end); put("\"");        }
  virtual void do_ulong(unsigned long long &i)   override { queue_char(','); put(i);                            }
  virtual void do_long(long long &i)             override { queue_char(','); put(i);                            }
  virtual void do_float(float &f)                override { queue_char(','); put(f);                            }
  virtual void do_bool(bool &b)                  override { queue_char(','); put(b ? "true" : "false");         }
  virtual void do_int(int &i)                    override { queue_char(','); put(i);                            }

  virtual void node_end()                override { }

  virtual void end()                     override {                  unindent(); print_indent();  put("}");                          }
};

struct JsonDeserializer : public IDeserializer {
  char* owned_buffer = nullptr;
  char* _p = nullptr;
  RHash _name_hash = 0;
  bool enum_started = false;

  JsonDeserializer(FILE * f) {
    auto head = ftell(f);
    fseek(f, 0, SEEK_END);
    auto memsize = ftell(f) - head;
    fseek(f, head, SEEK_SET);

    char* buffer = new char[memsize + 1];
    fread(buffer, memsize, 1, f);
    buffer[memsize] = 0;
    fseek(f, head, SEEK_SET);

    _p = buffer;
  }

  JsonDeserializer(char* json) {
    _p = json;
  }

  ~JsonDeserializer() {
    if (owned_buffer) {
      delete owned_buffer;
      owned_buffer = nullptr;
    }
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

  virtual RHash hash_key() override { return _name_hash; }

  virtual bool next_key() override {
    char c = current();
    if (c == '}') {
      expect('}');  // document end
      return false;
    }

    expect('\"');
    char* end = seek('\"');
    if (*end != 0) {
      _name_hash = rose::hash_fnv(_p, end);
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
    _name_hash = rose::hash_fnv(_p, end);
    _p = end;
    return true;
  }

  virtual void skip_key() override { assert(false); /*TODO*/ }

  virtual bool in_array() override {
    char c = current();
    if (c == ']') {
      expect(']');
      return false;
    }
    return true;
  }

  virtual void do_string(char* s, char* end) {
    trim();
    expect('\"');
    char hex[3] = {0};

    for(;s != end; ++s) {
        char c = *_p;
        if (c == '\"') {
            expect('\"');
            break;
        }
        else if(c == '\\') {
            _p++;
            c = *_p; //character after the '\'
            switch(c) {
                case '\\': *s = '\\'; break;
                case '/':  *s = '/'; break;
                case 'n':  *s = '\n'; break;
                case 'r':  *s =  '\r'; break;
                case 'b':  *s = '\b'; break;
                case 'f':  *s = '\f'; break;
                case 't':  *s = '\t'; break;
                case 'u':
                    expect('0');
                    expect('0');
                    _p++;
                    assert(*_p);
                    hex[0] = *(_p); /*TODO check range*/
                    _p++;
                    assert(*_p);
                    hex[1] = *(_p); /*TODO check range*/
                    _p++;
                    *s = (char)strtol(hex, NULL, 16);
            }
        } else {
            *s = *_p;
        }

        _p++;
    }

    for (; s != end; ++s) {
        *s = 0;
    }
  }

  virtual void do_bool(bool& b) override {
    char c = current();

    if(c == 't') {
      expect('t'); expect('r'); expect('u'); expect('e');
      b = true;
    } else if(c == 'f') {
      expect('f'); expect('a'); expect('l'); expect('s'); expect('e');
      b = false;
    } else {
      std::cerr << "Error: expected '"
                << "true or false"
                << "' but got "
                << c
                << ".\n";
    }
  }

  virtual void do_float(float& f) override {
    trim();
    f = strtof(_p, &_p);
  }

  virtual void do_int(int& i) override {
    trim();
    i = (int)strtol(_p, &_p, 10);
  }

  virtual void do_long(long long& i) override {
    trim();
    i = strtoll(_p, &_p, 10);
  }

  virtual void do_ulong(unsigned long long& i) override {
    trim();
    i = strtoull(_p, &_p, 10);
  }
};
