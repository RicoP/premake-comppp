#pragma once

#include "serializer.h"

struct ImguiSerializer : public ISerializer {
  ros::hash_value _type_hash = 0;
  const char * _key_name = "ROOT";

  virtual bool node_begin(const char *type, ros::hash_value type_hash, void *p) override {
    assert(ros::hash(type) == type_hash);

    ImGui::PushID(_key_name);
    _type_hash = type_hash;

    switch (_type_hash) {
      case ros::hash("vector3"):
        ImGui::InputFloat3(_key_name, reinterpret_cast<float *>(p));
        return false;
      default:
        break;
    }

    //if(!custom_type) {
    //  ImGui::PushID(name_hash);
    //  if (ImGui::TreeNode(name)) {
    //    Serializer<T, ImguiSerializer>::serialize(o, *this);
    //    ImGui::TreePop();
    //  }
    //  ImGui::PopID();
    //}

    return ImGui::TreeNode(type);
  }

  virtual void key(const char *name) override {
    _key_name = name;
    // ImGui::Text(name);
    // ImGui::SameLine();

    //ImGui::TreePop(); //???
  }

  virtual void begin_array() override {
  }
  virtual void end_array() override {
  }
  virtual void write_enum(const char *) override {
  }
  virtual void end_enum() override {
  }
  virtual void do_string(char *begin, char *end) override {
    ptrdiff_t diff = end - begin;

    ImGui::InputText(_key_name, begin, diff);
  }
  virtual void do_bool(bool &b) override {
    ImGui::Checkbox(_key_name, &b);
  }
  virtual void do_float(float &f) override {
    ImGui::InputFloat(_key_name, &f);
  }
  virtual void do_int(int &i) override {
    ImGui::InputInt(_key_name, &i);
  }
  virtual void node_end() override {
    ImGui::TreePop();
  }
  virtual void end() override {
      ImGui::PopID();
  }
};
