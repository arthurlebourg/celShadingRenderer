#include "program.hh"

int main()
{
    std::string shd_vertex = "shaders/vertex.shd";
    std::string shd_fragment = "shaders/fragment.shd";

    GLFWwindow *window = init_window();

    Object plane("objects/plane.obj", "textures/white.tga", glm::vec3(0, 0, 0),
                 0.0);
    Object amogus("objects/amongus.obj", "textures/pierre_sang.tga",
                  glm::vec3(0, 30, 30), 1.0);

    auto scene = std::make_shared<Scene>(glm::vec3(0.0, 10.0, 0.0));
    auto player =
        std::make_shared<Player>(glm::vec3(0, 10, 0.0), glm::vec3(0, 0, -1));
    scene->add_player(player);
    scene->add_object(std::make_shared<Object>(plane));
    scene->add_object(std::make_shared<Object>(amogus));

    std::shared_ptr<Program> prog =
        std::make_shared<Program>(shd_vertex, shd_fragment, window, scene);

    set_prog_var(prog);
    if (!prog->is_ready())
    {
        std::cerr << "error building prog" << std::endl;
    }

    prog->display();

    glfwTerminate();

    return 0;
}
