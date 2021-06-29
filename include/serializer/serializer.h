#pragma once

#include <rose/container.h>
#include <rose/hash.h>

namespace rose {
template <size_t N>
struct string {
  char data[N];
};

template <size_t N, size_t M>
inline bool operator==(const rose::string<N> &lhs, const rose::string<M> &rhs) {
  return strcmp(lhs.data, rhs.data) == 0;
}

template <size_t N>
hash_value hash(string<N> s) {
  return hash_fnv(s.data, s.data + N);
}
}  // namespace rose

class IDeserializer {
 public:
  typedef rose::hash_value hash;

  virtual bool next_key() = 0;
  virtual void skip_key() = 0;
  virtual hash hash_key() = 0;

  virtual bool in_array() = 0;

  virtual bool in_enum() = 0;

  virtual void do_string(char *begin, char *end) = 0;
  virtual void do_bool(bool &) = 0;
  virtual void do_float(float &) = 0;
  virtual void do_int(int &) = 0;
  virtual void do_long(long long &) = 0;
  virtual void do_ulong(unsigned long long &) = 0;
};

class ISerializer {
 public:
  virtual bool node_begin(const char *name, rose::hash_value name_hash, void *) = 0;

  virtual void key(const char *) = 0;

  virtual bool begin_array() = 0;
  virtual void in_array(size_t) = 0;
  virtual void end_array() = 0;

  virtual void write_enum(const char *) = 0;
  virtual void end_enum() = 0;

  virtual void do_string(char *begin, char *end) = 0;
  virtual void do_bool(bool &) = 0;
  virtual void do_float(float &) = 0;
  virtual void do_int(int &) = 0;
  virtual void do_long(long long &) = 0;
  virtual void do_ulong(unsigned long long &) = 0;

  virtual void node_end() = 0;

  virtual void end() = 0;
};

namespace rose {
namespace ecs {

  template <size_t N>
  inline void serialize(rose::string<N> &o, ISerializer &s) {
    s.do_string(o.data, o.data + N);
  }
  template <size_t N>
  inline void deserialize(rose::string<N> &o, IDeserializer &d) {
    d.do_string(o.data, o.data + N);
  }

  inline void serialize(bool &b, ISerializer &s) { s.do_bool(b); }
  inline void deserialize(bool &b, IDeserializer &d) { d.do_bool(b); }

  inline void serialize(int &i, ISerializer &s) { s.do_int(i); }
  inline void deserialize(int &i, IDeserializer &d) { d.do_int(i); }

  inline void serialize(long long &i, ISerializer &s) { s.do_long(i); }
  inline void deserialize(long long &i, IDeserializer &d) { d.do_long(i); }

  inline void serialize(unsigned long long &i, ISerializer &s) { s.do_ulong(i); }
  inline void deserialize(unsigned long long &i, IDeserializer &d) { d.do_ulong(i); }

  inline void serialize(float &f, ISerializer &s) { s.do_float(f); }
  inline void deserialize(float &f, IDeserializer &d) { d.do_float(f); }

  inline void randomize(float &o, rose::hash_value &h) {
    rose::next(h);
    o = (((int)(h % 400)) - 200) * .5f;
  }

  inline void randomize(int &o, rose::hash_value &h) {
    rose::next(h);
    o = (int)(h % 100);
  }

  inline void randomize(long long &o, rose::hash_value &h) {
    rose::next(h);
    o = (long long)(h % 100);
  }

  inline void randomize(unsigned long long &o, rose::hash_value &h) {
    rose::next(h);
    o = (unsigned long long)(h % 100);
  }

  inline void randomize(bool &o, rose::hash_value &h) {
    rose::next(h);
    o = h % 2 == 0;
  }

  template <size_t N, class T>
  inline void randomize(rose::vectorPOD<N, T> &o, rose::hash_value &h) {
    size_t length = h % N;
    o.size = length;
    for (size_t i = 0; i != length; ++i) {
      randomize(o.elements[i], h);
    }
  }

  template <size_t N, class T>
  inline void randomize(T (&o)[N], rose::hash_value &h) {
    for (size_t i = 0; i != N; ++i) {
      randomize(o[i], h);
    }
  }

  template <size_t N>
  inline void randomize(rose::string<N> &o, rose::hash_value &h) {
    rose::next(h);

    std::memset(o.data, 0, N);

    size_t length = h % N - 1;
    for (size_t i = 0; i != length; ++i) {
      rose::next(h);
      o.data[i] = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"[h % 51];
    }
  }

  template <size_t N, class T>
  inline void serialize(T (&o)[N], ISerializer &s) {
    if (s.begin_array()) {
      for (size_t i = 0; i != N; ++i) {
        // assert(i < N);
        s.in_array(i);
        serialize(o[i], s);
      }
      s.end_array();
    }
  }

  template <size_t N, class T>
  inline void serialize(rose::vectorPOD<N, T> &o, ISerializer &s) {
    if (s.begin_array()) {
      for (size_t i = 0; i != o.size; ++i) {
        // assert(i < N);
        s.in_array(i);
        serialize(o.elements[i], s);
      }
      s.end_array();
    }
  }

  template <size_t N, class T>
  inline void deserialize(rose::vectorPOD<N, T> &o, IDeserializer &d) {
    o.size = 0;
    while (d.in_array()) {
      // assert(o.size < N);
      deserialize(o.elements[o.size], d);
      ++o.size;
    }
  }

  template <size_t N, class T>
  inline void deserialize(T (&o)[N], IDeserializer &d) {
    size_t size = 0;
    while (d.in_array() && size != N) {
      // assert(o.size < N);
      deserialize(o[size], d);
      ++size;
    }
  }

}  // namespace ecs
}  // namespace rose
