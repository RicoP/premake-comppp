#pragma once

#include <array>


namespace ros {

typedef unsigned long long hash_value;

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


class IDeserializer {
 public:
  virtual bool next() = 0;
  virtual void skip() = 0;
  virtual ros::hash_value name_hash() = 0;
};

class ISerializer {
 public:
  virtual void hint_type(const char *) = 0;
  virtual void set_field_name(const char *) = 0;
  virtual void end() = 0;
};

namespace ros {
template <size_t N, class T>
struct array {
  size_t size;

  bool operator==(const array &rhs) const { return true; }
};

template <size_t N, class T>
inline hash_value hash(const array<N,T> & v);
}

template <size_t N, class T>
inline void serialize(ros::array<N, T> &o, ISerializer &s);

template <size_t N, class T>
inline void deserialize(ros::array<N, T> &o, IDeserializer &s);


inline void serialize(int, ISerializer &);
inline void deserialize(int, IDeserializer &);