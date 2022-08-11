#pragma once

#include <fstream>

#include "image.hh"
#include "image_io.hh"
#include "player.hh"
#include "scene.hh"
#include "utils.hh"

GLFWwindow *init_window();

class Program
{
public:
    Program(std::string &vertex_shader_src, std::string &fragment_shader_src,
            GLFWwindow *window, std::shared_ptr<Scene> scene);

    ~Program();

    char *get_log();

    bool is_ready();

    void use();

    void display();

    void set_mat4_uniform(const char *name, glm::mat4 mat);
    void set_mat4_uniform(const char *name, btScalar *mat);
    void set_vec3_uniform(const char *name, glm::vec3 vec);

    GLFWwindow *get_window();

    unsigned int get_shader();

    std::shared_ptr<Scene> get_scene();

    void update_position();

    void update_physics(const float deltaTime);
    void draw(glm::mat4 const &model_view_matrix,
              glm::mat4 const &projection_matrix, bool clip);

    void render(glm::mat4 const &model_view_matrix,
                glm::mat4 const &projection_matrix);

    void render_portals(glm::mat4 const &view_mat, glm::mat4 const &proj_mat,
                        unsigned int recursion_level, bool clip);

private:
    unsigned int shader_program_;
    std::shared_ptr<Scene> scene_;

    unsigned int max_recursion_level_ = 2;

    GLFWwindow *window_;

    unsigned int vertex_shader_;
    unsigned int fragment_shader_;

    unsigned int VAO;

    char log[512];

    bool ready_;
};

void set_prog_var(std::shared_ptr<Program> p);
