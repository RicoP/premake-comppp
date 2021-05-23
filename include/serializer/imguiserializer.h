#pragma once

#include "serializer.h"
#include <game/util/mathhelper.h>

struct ImguiSerializer : public ISerializer {
  ros::hash_value _type_hash = 0;
  const char* _key_name = 0;
  const char* _array_key = 0;
  char _array_full_name[128] = {0};
  int depth = 0;

  virtual bool custom_type(const char *type, ros::hash_value type_hash, void *p) {
    return false;
  }

  virtual bool node_begin(const char *type, ros::hash_value type_hash,
                          void *p) override {
    assert(ros::hash(type) == type_hash);

    _type_hash = type_hash;

    if(custom_type(type, type_hash, p)) return false;

    if(depth++ == 0) return true; //ROOT is always open
    else  {
        bool inside = ImGui::TreeNode(_key_name);
        if(!inside) depth--;
        return inside;
    }
  }

  virtual void key(const char *name) override { _key_name = name; }
  virtual bool begin_array() override { _array_key = _key_name; return ImGui::TreeNode(_key_name); }
  virtual void in_array(size_t iz) override { sprintf(_array_full_name, "%s[%d]", _array_key, (int)iz); _key_name = _array_full_name; }
  virtual void end_array() override { _key_name = 0; ImGui::TreePop(); }
  virtual void write_enum(const char *) override {}
  virtual void end_enum() override {}
  virtual void do_string(char *begin, char *end) override {
    ptrdiff_t diff = end - begin;
    ImGui::InputText(_key_name, begin, diff);
  }
  virtual void do_bool(bool &b) override { ImGui::Checkbox(_key_name, &b); }
  virtual void do_float(float &f) override { ImGui::InputFloat(_key_name, &f); }
  virtual void do_int(int &i) override { ImGui::InputInt(_key_name, &i); }
  virtual void do_long(long long &l) override {
    int i = (int)l;
    if (ImGui::InputInt(_key_name, &i)) {
      l = i;
    }
  }
  virtual void do_ulong(unsigned long long &l) override {
    int i = (int)l;
    if (ImGui::InputInt(_key_name, &i)) {
      l = i;
    }
  }
  virtual void node_end() override {
    depth--;
    if (depth != 0) ImGui::TreePop();
  }
  virtual void end() override { }
};
