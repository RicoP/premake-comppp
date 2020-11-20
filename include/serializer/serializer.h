#pragma once

#include <ros/hash.h>
#include <ros/container.h>

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
  virtual bool node_begin(const char *name, ros::hash_value name_hash,
                          void *) = 0;

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
  // virtual void do_long(long long) = 0;

  virtual void node_end() = 0;

  virtual void end() = 0;
};

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
inline void randomize(rose::vectorPOD<N, T> &o, ros::hash_value &h) {
  ros::next(h);

  size_t length = h % N;
  o.size = length;
  for (size_t i = 0; i != length; ++i) {
    randomize(o.elements[i], h);
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
