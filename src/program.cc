#include "program.hh"

#include <fstream>

std::shared_ptr<Program> p;

int old_pos_x = 0;
int old_pos_y = 0;
int win_w = 1024;
int win_h = 1024;
bool firstMouse = true;
bool key_states[256];

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void mouse_motion_callback(int x, int y)
{
    x = x;
    y = y;
    /*if (firstMouse)
    {
        old_pos_x = x;
        old_pos_y = y;
        firstMouse = false;
    }

    float xoffset = x - old_pos_x;
    float yoffset = old_pos_y - y;
    old_pos_x = x;
    old_pos_y = y;

    float sensitivity = 0.05f;
    xoffset *= deltaTime * sensitivity;
    yoffset *= deltaTime * sensitivity;

    p->get_player()->add_yaw(xoffset);
    p->get_player()->add_pitch(yoffset);

    float dir_x = cos(glm::radians(p->get_player()->get_yaw()))
        * cos(glm::radians(p->get_player()->get_pitch()));
    float dir_y = sin(glm::radians(p->get_player()->get_pitch()));
    float dir_z = sin(glm::radians(p->get_player()->get_yaw()))
        * cos(glm::radians(p->get_player()->get_pitch()));

    p->get_player()->set_direction(glm::vec3(dir_x, dir_y, dir_z));

    p->get_player()->normalize_direction();

    // this is the main thing that keeps it from leaving the screen
    if (x < 100 || x > win_w - 100)
    { // you can use values other than 100 for the screen edges if you like,
      // kind of seems to depend on your mouse sensitivity for what ends up
      // working best
        old_pos_x =
            win_w / 2; // centers the last known position, this way there isn't
                       // an odd jump with your cam as it resets
        old_pos_y = win_h / 2;
        glutWarpPointer(win_w / 2, win_h / 2); // centers the cursor
    }
    else if (y < 100 || y > win_h - 100)
    {
        old_pos_x = win_w / 2;
        old_pos_y = win_h / 2;
        glutWarpPointer(win_w / 2, win_h / 2);
    }
    */
}

void keyboard_keyup(unsigned char key, int, int)
{
    key_states[key] = false;
}

void keyboard_keydown(unsigned char key, int, int)
{
    key_states[key] = true;
}

void framebuffer_size_callback(GLFWwindow *, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

void display()
{
    float currentFrame = 0; // glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    /*p->get_scene()->update_physics(deltaTime, p->get_player());
    p->get_player()->set_speed(key_states['a']);
    p->get_player()->move(key_states['z'] - key_states['s'],
                          key_states['d'] - key_states['q'], deltaTime);

    p->get_scene()->draw(p->shader_program_, p->get_player()->get_model_view(),
                         p->get_player()->get_projection(),
                         key_states['p']);
    */
}

Program::Program(std::string vertex_shader_src, std::string fragment_shader_src)
{
    ready_ = false;
    vertex_shader_src = vertex_shader_src;
    fragment_shader_src = fragment_shader_src;
    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    shader_program_ = glCreateProgram();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window_ = glfwCreateWindow(win_w, win_h, "LearnOpenGL", NULL, NULL);
    if (window_ == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    ready_ = true;
}

Program::~Program()
{
    glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);
    glfwTerminate();
}

char *Program::get_log()
{
    return log;
}

bool Program::is_ready()
{
    return ready_;
}

void Program::use()
{
    glUseProgram(shader_program_);
}

void Program::set_mat4_uniform(const char *name, glm::mat4 mat)
{
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Program::set_mat4_uniform(const char *name, btScalar *mat)
{
    float res[16];
    for (int i = 0; i < 16; i++)
    {
        res[i] = (float)mat[i];
    }
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, res);
}

void Program::set_vec3_uniform(const char *name, glm::vec3 vec)
{
    GLint location = glGetUniformLocation(shader_program_, name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

GLFWwindow *Program::get_window()
{
    return window_;
}

/*std::shared_ptr<Scene> Program::get_scene()
{
    return scene_;
}

std::shared_ptr<Player> Program::get_player()
{
    return player_;
}*/
