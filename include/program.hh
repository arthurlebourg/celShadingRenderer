#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>

#include "image.hh"
#include "image_io.hh"
#include "utils.hh"
#include <GLFW/glfw3.h>

class Program {
public:
  Program();

  ~Program();

  static std::shared_ptr<Program>
  make_program(std::string &vertex_shader_src,
               std::string &fragment_shader_src);

  char *get_log();

  bool is_ready();

  void use();

  void set_mat4_uniform(const char *name, glm::mat4 mat);
  void set_mat4_uniform(const char *name, btScalar *mat);
  void set_vec3_uniform(const char *name, glm::vec3 vec);

  unsigned int get_shader();

  void update_position();

private:
  unsigned int shader_program_;

  unsigned int vertex_shader_;
  unsigned int fragment_shader_;

  unsigned int VAO;

  char log[512];

  bool ready_;
};
