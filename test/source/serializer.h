#pragma once

#include <array>

namespace ros {

typedef unsigned long long hash_value;

constexpr hash_value hash_fnv(const char *pBuffer, const char *end) {
  constexpr hash_value MagicPrime = 0x00000100000001b3;
  hash_value Hash = 0xcbf29ce484222325ULL;

  for (; pBuffer != end; pBuffer++) Hash = (Hash ^ *pBuffer) * MagicPrime;

  return Hash;
}

constexpr hash_value fnFNV(const char *pBuffer) {
  constexpr hash_value MagicPrime = 0x00000100000001b3;
  hash_value Hash = 0xcbf29ce484222325ULL;

  for (; *pBuffer; pBuffer++) Hash = (Hash ^ *pBuffer) * MagicPrime;

  return Hash;
}

constexpr hash_value hash(const char *value) { return fnFNV(value); }

// http://www.jstatsoft.org/article/view/v008i14/xorshift.pdf page 4
// https://en.wikipedia.org/wiki/Xorshift#xorshift*
constexpr hash_value xor64(hash_value h) {
  h ^= 88172645463325252ULL;  // xor with a constant so a seed of 0 will not
                              // result in a infinite loop
  h ^= h >> 12;
  h ^= h << 25;
  h ^= h >> 27;
  return h * 0x2545F4914F6CDD1DULL;
}

namespace internal {
template <class T>
inline hash_value hash_simple(T value) {
  static_assert(sizeof(T) <= sizeof(hash_value),
                "sizeof(T) can't be bigger than sizeof(hash_value)");
  union {
    hash_value u_h;
    T u_f;
  };
  u_h = 0;
  u_f = value;
  return u_h;
}
}  // namespace internal

constexpr void next(hash_value &h) { h = xor64(h); }

inline float nextf(hash_value &h) {
  next(h);

  union {
    hash_value u_x;
    float u_f;
  };

  // Manipulate the exponent and fraction of the floating point number
  // in a way, that we get a number from 1 (inlcusive) and 2 (exclusive).
  u_x = h;
  u_x &= 0x007FFFFF007FFFFF;
  u_x |= 0x3F8000003F800000;

  return u_f - 1.0f;
}

constexpr hash_value hash(unsigned char v) { return v; }
constexpr hash_value hash(unsigned int v) { return v; }
constexpr hash_value hash(unsigned long int v) { return v; }
constexpr hash_value hash(unsigned long long int v) { return v; }
constexpr hash_value hash(unsigned short int v) { return v; }

constexpr hash_value hash(bool v) { return v ? 1 : 0; }

inline hash_value hash(signed char v) { return internal::hash_simple(v); }
inline hash_value hash(int v) { return internal::hash_simple(v); }
inline hash_value hash(long int v) { return internal::hash_simple(v); }
inline hash_value hash(long long int v) { return internal::hash_simple(v); }
inline hash_value hash(short int v) { return internal::hash_simple(v); }

inline hash_value hash(double v) { return internal::hash_simple(v); }
inline hash_value hash(float v) { return internal::hash_simple(v); }
// inline hash_value hash(long double v) { return internal::hash_simple(v); }
inline hash_value hash(wchar_t v) { return internal::hash_simple(v); }
}  // namespace ros

namespace ros {
template <size_t N>
struct string {
  char data[N];
};

template <size_t N>
hash_value hash(string<N> s) {
  ros::hash_fnv(s.data, s.data + N);
}
}  // namespace ros

template <size_t N, size_t M>
inline bool operator==(const ros::string<N> &lhs, const ros::string<M> &rhs) {
  return strcmp(lhs.data, rhs.data) == 0;
}

class IDeserializer {
 public:
  typedef ros::hash_value hash;

  virtual bool next_key() = 0;
  virtual void skip_key() = 0;
  virtual hash hash_key() = 0;

  virtual bool in_array() = 0;

  virtual bool in_enum() = 0;

  virtual void do_string(char *begin, char *end) = 0;
  virtual void do_bool(bool &) = 0;
  virtual void do_float(float &) = 0;
  virtual void do_int(int &) = 0;
  // virtual void do_long(long long &) = 0;
};

class ISerializer {
 public:
  virtual bool node_begin(const char *name, ros::hash_value name_hash, void *) = 0;

  virtual void key(const char *) = 0;

  virtual void begin_array() = 0;
  virtual void begin_array_element(size_t) = 0;
  virtual void end_array_element() = 0;
  virtual void end_array() = 0;

  virtual void write_enum(const char *) = 0;
  virtual void end_enum() = 0;

  virtual void do_string(char *begin, char *end) = 0;
  virtual void do_bool(bool&) = 0;
  virtual void do_float(float&) = 0;
  virtual void do_int(int&) = 0;
  // virtual void do_long(long long) = 0;

  virtual void node_end() = 0;

  virtual void end() = 0;
};

namespace ros {
template <size_t N, class T>
struct array {
  size_t size;
  T values[N];

  bool operator==(const array &rhs) const {
    if (size != rhs.size) return false;
    for (size_t i = 0; i != size; ++i) {
      if (!(values[i] == rhs.values[i])) return false;
    }
    return true;
  }

  template <size_t N>
  hash_value hash(ros::string<N> &s) {
    return ros::hash_fnv(o.data, o.data + N);
  }
};

template <size_t N, class T>
inline hash_value hash(const array<N, T> &v);
}  // namespace ros

// inline void serialize(long long &i, ISerializer &s) { s.do_long(i); }
// inline void deserialize(long long &i, IDeserializer &d) { d.do_long(i); }

template <size_t N>
inline void serialize(ros::string<N> &o, ISerializer &s) {
  s.do_string(o.data, o.data + N);
}
template <size_t N>
inline void deserialize(ros::string<N> &o, IDeserializer &d) {
  d.do_string(o.data, o.data + N);
}

inline void serialize(bool &b, ISerializer &s) { s.do_bool(b); }
inline void deserialize(bool &b, IDeserializer &d) { d.do_bool(b); }

inline void serialize(int &i, ISerializer &s) { s.do_int(i); }
inline void deserialize(int &i, IDeserializer &d) { d.do_int(i); }

inline void serialize(float &f, ISerializer &s) { s.do_float(f); }
inline void deserialize(float &f, IDeserializer &d) { d.do_float(f); }

inline void randomize(float &o, ros::hash_value &h) {
  ros::next(h);
  o = (((int)(h % 400)) - 200) * .5f;
}

inline void randomize(int &o, ros::hash_value &h) {
  ros::next(h);
  o = (int)(h % 100);
}

inline void randomize(bool &o, ros::hash_value &h) {
  ros::next(h);
  o = h % 2 == 0;
}

template <size_t N, class T>
inline void randomize(ros::array<N, T> &o, ros::hash_value &h) {
  ros::next(h);

  size_t length = h % N;
  o.size = length;
  for (size_t i = 0; i != length; ++i) {
    randomize(o.values[i], h);
  }
}

template <size_t N>
inline void randomize(ros::string<N> &o, ros::hash_value &h) {
  ros::next(h);

  std::memset(o.data, 0, N);

  size_t length = h % N - 1;
  for (size_t i = 0; i != length; ++i) {
    ros::next(h);
    o.data[i] = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"[h % 51];
  }
}

template <size_t N, class T>
inline void serialize(ros::array<N, T> &o, ISerializer &s) {
  s.begin_array();
  for (size_t i = 0; i != o.size; ++i) {
    //assert(i < N);
    s.begin_array_element(i);
    serialize(o.values[i], s);
    s.end_array_element();
  }
  s.end_array();
}

template <size_t N, class T>
inline void deserialize(ros::array<N, T> &o, IDeserializer &d) {
  o.size = 0;
  while (d.in_array()) {
    deserialize(o.values[o.size], d);
    ++o.size;
  }
}
