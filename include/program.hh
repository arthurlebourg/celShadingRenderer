#pragma once

#include <fstream>

#include "image.hh"
#include "image_io.hh"
#include "player.hh"
#include "scene.hh"
#include "shader.hh"

GLFWwindow *init_window();

class Program
{
public:
    Program(std::string &vertex_shader_src, std::string &fragment_shader_src,
            std::string &fragment_single_color_src, GLFWwindow *window,
            std::shared_ptr<Scene> scene);

    ~Program();

    bool is_ready();

    void use();

    void display();

    GLFWwindow *get_window();

    std::shared_ptr<Scene> get_scene();

    void update_position();

    void update_physics(const float deltaTime);

    void render(glm::mat4 const &model_view_matrix,
                glm::mat4 const &projection_matrix);

private:
    std::shared_ptr<Scene> scene_;

    GLFWwindow *window_;

    Shader render_shader_;
    Shader single_color_;

    bool ready_;
};

void set_prog_var(std::shared_ptr<Program> p);
