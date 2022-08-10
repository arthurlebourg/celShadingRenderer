#include "program.hh"

int main()
{
    std::string shd_vertex = "shaders/vertex.shd";
    std::string shd_fragment = "shaders/fragment.shd";

    std::shared_ptr<Program> p =
        std::make_shared<Program>(shd_vertex, shd_fragment);

    if (!p->is_ready())
    {
        std::cerr << "error building prog" << std::endl;
    }

    p->display();

    glfwTerminate();

    return 0;
}
