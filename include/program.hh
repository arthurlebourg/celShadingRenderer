#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "glad/glad.h"
#include "image.hh"
#include "image_io.hh"
#include "utils.hh"

class Program
{
public:
    Program(std::string vertex_shader_src, std::string fragment_shader_src);

    ~Program();

    char *get_log();

    bool is_ready();

    void use();

    void set_mat4_uniform(const char *name, glm::mat4 mat);
    void set_mat4_uniform(const char *name, btScalar *mat);
    void set_vec3_uniform(const char *name, glm::vec3 vec);

    GLFWwindow *get_window();

    unsigned int get_shader();

    void update_position();

private:
    unsigned int shader_program_;

    GLFWwindow *window_;

    unsigned int vertex_shader_;
    unsigned int fragment_shader_;

    unsigned int VAO;

    char log[512];

    bool ready_;
};
