#pragma once

#include "serializer.h"

struct ImguiSerializer : public ISerializer {
  ros::hash_value _name_hash = 0;
  void *_p = 0;
  const char *_key_name = 0;
  bool custom_type = false;

  virtual void begin(const char *name, ros::hash_value name_hash,
                     void *p) override {
    assert(ros::hash(name) == name_hash);

    _name_hash = name_hash;
    _p = p;

    custom_type = true;
    switch (_name_hash) {
      case ros::hash("vector3"):
        ImGui::InputFloat3(_key_name, reinterpret_cast<float *>(p));
        break;
      default:
        custom_type = false;
        break;
    }
  }

  virtual void key(const char *name) override {
    if (custom_type) return;

    _key_name = name;
    // ImGui::Text(name);
    // ImGui::SameLine();
  }

  virtual void begin_array() override {
    if (custom_type) return;
  }
  virtual void end_array() override {
    if (custom_type) return;
  }
  virtual void write_enum(const char *) override {
    if (custom_type) return;
  }
  virtual void end_enum() override {
    if (custom_type) return;
  }
  virtual void do_string(char *begin, char *end) override {
    if (custom_type) return;

    ptrdiff_t diff = end - begin;

    ImGui::InputText(_key_name, begin, diff);
  }
  virtual void do_bool(bool &b) override {
    if (custom_type) return;

    ImGui::Checkbox(_key_name, &b);
  }
  virtual void do_float(float &f) override {
    if (custom_type) return;

    ImGui::InputFloat(_key_name, &f);
  }
  virtual void do_int(int &i) override {
    if (custom_type) return;

    ImGui::InputInt(_key_name, &i);
  }

  virtual void end() override {
    if (custom_type) return;
  }
};
