#include "program.hh"

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    std::shared_ptr<Program> p = std::make_shared<Program>("bite", "bite");

    while (!glfwWindowShouldClose(p->get_window()))
    {
        processInput(p->get_window());

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(p->get_window());
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
