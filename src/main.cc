#include "program.hh"

int main()
{
    std::string shd_vertex = "shaders/vertex.shd";
    std::string shd_fragment = "shaders/fragment.shd";

    auto scene = std::make_shared<Scene>(glm::vec3(25.0, 60.0, 0.0));
    auto player =
        std::make_shared<Player>(glm::vec3(10, 10, 3.0), glm::vec3(0, 0, -1));
    scene->add_player(player);
    std::shared_ptr<Program> p =
        std::make_shared<Program>(shd_vertex, shd_fragment, scene);

    if (!p->is_ready())
    {
        std::cerr << "error building prog" << std::endl;
    }

    p->display();

    glfwTerminate();

    return 0;
}
