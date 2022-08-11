#include "program.hh"

double old_pos_x = 0;
double old_pos_y = 0;
int win_w = 1024;
int win_h = 1024;
bool firstMouse = true;
bool key_states[256];

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

std::shared_ptr<Program> prog;

void set_prog_var(std::shared_ptr<Program> p)
{
    prog = p;
}

void mouse_motion_callback(GLFWwindow *, double x, double y)
{
    x = x;
    y = y;
    if (firstMouse)
    {
        old_pos_x = x;
        old_pos_y = y;
        firstMouse = false;
    }

    float xoffset = x - old_pos_x;
    float yoffset = old_pos_y - y;
    old_pos_x = x;
    old_pos_y = y;

    float sensitivity = 10.0f;
    xoffset *= deltaTime * sensitivity;
    yoffset *= deltaTime * sensitivity;

    auto scene_ = prog->get_scene();
    scene_->get_player()->add_yaw(xoffset);
    scene_->get_player()->add_pitch(yoffset);

    float dir_x = cos(glm::radians(scene_->get_player()->get_yaw()))
        * cos(glm::radians(scene_->get_player()->get_pitch()));
    float dir_y = sin(glm::radians(scene_->get_player()->get_pitch()));
    float dir_z = sin(glm::radians(scene_->get_player()->get_yaw()))
        * cos(glm::radians(scene_->get_player()->get_pitch()));

    scene_->get_player()->set_direction(glm::vec3(dir_x, dir_y, dir_z));

    scene_->get_player()->normalize_direction();

    // this is the main thing that keeps it from leaving the screen
    if (x < 50 || x > win_w - 50)
    { // you can use values other than 100 for the screen edges if you like,
      // kind of seems to depend on your mouse sensitivity for what ends up
      // working best
        old_pos_x =
            win_w / 2; // centers the last known position, this way there isn't
                       // an odd jump with your cam as it resets
        old_pos_y = win_h / 2;
        glfwSetCursorPos(prog->get_window(), win_w / 2, win_h / 2);
        // glutWarpPointer(win_w / 2, win_h / 2); // centers the cursor
    }
    else if (y < 50 || y > win_h - 50)
    {
        old_pos_x = win_w / 2;
        old_pos_y = win_h / 2;
        glfwSetCursorPos(prog->get_window(), win_w / 2, win_h / 2);
        // glutWarpPointer(win_w / 2, win_h / 2);
    }
}

void keyboard_callback(GLFWwindow *, int key, int, int action, int)
{
    if (action == GLFW_PRESS)
    {
        key_states[key] = true;
        std::cout << (char)key << std::endl;
    }
    else if (action == GLFW_RELEASE)
    {
        key_states[key] = false;
    }
}

void framebuffer_size_callback(GLFWwindow *, int width, int height)
{
    win_w = width;
    win_h = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Program::display()
{
    while (!glfwWindowShouldClose(window_))
    {
        float currentFrame = glfwGetTime(); // glutGet(GLUT_ELAPSED_TIME);
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update_physics(deltaTime);
        scene_->get_player()->set_speed(key_states['Q']);
        scene_->get_player()->move(key_states['W'] - key_states['S'],
                                   key_states['D'] - key_states['A'],
                                   deltaTime);

        draw(scene_->get_player()->get_model_view(),
             scene_->get_player()->get_projection(), key_states['p']);

        processInput(window_);

        glfwPollEvents();
    }
}

std::shared_ptr<Scene> Program::get_scene()
{
    return scene_;
}

GLFWwindow *init_window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window =
        glfwCreateWindow(win_w, win_h, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return window;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    return window;
}

Program::Program(std::string &vertex_shader_src,
                 std::string &fragment_shader_src, GLFWwindow *window,
                 std::shared_ptr<Scene> scene)
    : scene_(scene)
    , window_(window)
{
    ready_ = false;

    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    shader_program_ = glCreateProgram();

    int success;

    std::string vertex_shader_content = read_file(vertex_shader_src);
    std::string fragment_shader_content = read_file(fragment_shader_src);
    char *vertex_shd_src =
        (char *)std::malloc(vertex_shader_content.length() * sizeof(char));
    char *fragment_shd_src =
        (char *)std::malloc(fragment_shader_content.length() * sizeof(char));

    vertex_shader_content.copy(vertex_shd_src, vertex_shader_content.length());
    fragment_shader_content.copy(fragment_shd_src,
                                 fragment_shader_content.length());

    glShaderSource(vertex_shader_, 1, (const GLchar **)&(vertex_shd_src), NULL);
    glCompileShader(vertex_shader_);
    free(vertex_shd_src);

    glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_, 512, NULL, log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << log << std::endl;
        return;
    }

    glShaderSource(fragment_shader_, 1, (const GLchar **)&(fragment_shd_src),
                   NULL);
    glCompileShader(fragment_shader_);
    free(fragment_shd_src);

    glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_, 512, NULL, log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << log << std::endl;
        return;
    }

    glAttachShader(shader_program_, vertex_shader_);
    glAttachShader(shader_program_, fragment_shader_);
    glLinkProgram(shader_program_);

    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader_program_, 512, NULL, log);
        std::cout << "ERROR::SHADER::LINKAGE_FAILED\n" << log << std::endl;
        return;
    }

    glUseProgram(shader_program_);
    TEST_OPENGL_ERROR();

    glfwSetCursorPosCallback(window, mouse_motion_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    set_vec3_uniform("light_pos", scene->get_light());
    TEST_OPENGL_ERROR();

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

glm::mat4 portal_view(glm::mat4 orig_view, std::shared_ptr<Portal> src,
                      std::shared_ptr<Portal> dst)
{
    glm::mat4 mv = orig_view * src->get_transform();
    glm::mat4 portal_cam =
        // 3. transformation from source portal to the camera - it's the
        //    first portal's ModelView matrix:
        mv
        // 2. object is front-facing, the camera is facing the other way:
        * glm::rotate(glm::mat4(1.0), glm::radians(180.0f),
                      glm::vec3(0.0, 1.0, 0.0))
        // 1. go the destination portal; using inverse, because camera
        //    transformations are reversed compared to object
        //    transformations:
        * glm::inverse(dst->get_transform());
    return portal_cam;
}

/**
 * Checks whether the line defined by two points la and lb intersects
 * the portal.
 */
bool portal_intersection(glm::vec4 la, glm::vec4 lb,
                         std::shared_ptr<Portal> portal)
{
    if (la != lb)
    { // camera moved
      // Check for intersection with each of the portal's 2 front triangles
        for (int i = 0; i < 2; i++)
        {
            // Portal coordinates in world view
            glm::vec4 p0 = portal->get_transform()
                * glm::vec4(portal->get_vertices()[i * 3 + 0], 1.0);
            glm::vec4 p1 = portal->get_transform()
                * glm::vec4(portal->get_vertices()[i * 3 + 1], 1.0);
            glm::vec4 p2 = portal->get_transform()
                * glm::vec4(portal->get_vertices()[i * 3 + 2], 1.0);

            // Solve line-plane intersection using parametric form
            glm::vec3 tuv =
                glm::inverse(
                    glm::mat3(glm::vec3(la.x - lb.x, la.y - lb.y, la.z - lb.z),
                              glm::vec3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z),
                              glm::vec3(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z)))
                * glm::vec3(la.x - p0.x, la.y - p0.y, la.z - p0.z);
            float t = tuv.x, u = tuv.y, v = tuv.z;

            // intersection with the plane
            float lambda = 1e-6;
            if (t >= 0 - lambda && t <= 1 + lambda)
            {
                // intersection with the triangle
                if (u >= 0 - lambda && u <= 1 + lambda && v >= 0 - lambda
                    && v <= 1 + lambda && (u + v) <= 1 + lambda)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

float get_saclingFactor(glm::mat4 m)
{
    auto scalingFacotr =
        sqrt(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]);
    return scalingFacotr;
}

glm::mat3 get_rotationM(float scalingFacotr, glm::mat4 m)
{
    auto res = (1.0f / scalingFacotr)
        * glm::mat3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2],
                    m[2][0], m[2][1], m[2][2]);
    return glm::inverse(res);
}

void Program::update_physics(const float deltaTime)
{
    glm::mat4 prev_pos = scene_->get_player()->get_model_view();

    scene_->get_dynamic_world()->stepSimulation(deltaTime * 75.0f / 60.0f, 1);
    btTransform trans;
    trans.setIdentity();
    btRigidBody *player_body = scene_->get_player()->get_body();
    player_body->getMotionState()->getWorldTransform(trans);
    scene_->get_player()->set_position(trans.getOrigin().getX(),
                                       trans.getOrigin().getY(),
                                       trans.getOrigin().getZ());

    for (auto portal : scene_->get_portals())
    {
        glm::vec4 la = glm::inverse(prev_pos) * glm::vec4(0.0, 0.0, 0.0, 1.0);

        if (!is_sameSign(scene_->get_player()->get_position().x, la.x))
        {
            break;
        }

        glm::vec4 lb = glm::inverse(scene_->get_player()->get_model_view())
            * glm::vec4(0.0, 0.0, 0.0, 1.0);

        if (portal_intersection(la, lb, portal))
        {
            glm::mat4 new_trans_glm =
                portal_view(scene_->get_player()->get_model_view(), portal,
                            portal->get_destination());

            glm::mat4 new_world_perception = glm::inverse(new_trans_glm);
            glm::vec4 pos =
                new_world_perception * glm::vec4(0.0, 0.0, 0.0, 1.0);
            btTransform new_trans_bt;
            new_trans_bt.setIdentity();
            new_trans_bt.setOrigin(btVector3(pos.x, pos.y, pos.z));
            player_body->setWorldTransform(new_trans_bt);
            player_body->getMotionState()->setWorldTransform(new_trans_bt);

            scene_->get_player()->set_position(pos.x, pos.y, pos.z);

            if (abs(portal->get_angle()
                    - portal->get_destination()->get_angle())
                != 180)
            {
                auto backward = glm::vec3(new_world_perception[2][0],
                                          new_world_perception[2][1],
                                          new_world_perception[2][2]);
                scene_->get_player()->set_direction(-backward);
                scene_->get_player()->normalize_direction();

                auto new_yaw = acos(
                    scene_->get_player()->get_direction().x
                    / cos(glm::radians(scene_->get_player()->get_pitch())));
                scene_->get_player()->set_yaw(new_yaw * 180 / M_PI);
            }
            break;
        }
    }

    for (auto obj : scene_->get_objs())
    {
        trans.setIdentity();

        btRigidBody *body = obj->get_body();
        body->getMotionState()->getWorldTransform(trans);

        btScalar m[16];
        trans.getOpenGLMatrix(m);
        obj->set_transform(m);
    }
}

void Program::draw(glm::mat4 const &model_view_matrix,
                   glm::mat4 const &projection_matrix, bool clip)
{
    TEST_OPENGL_ERROR();
    glUseProgram(shader_program_);
    TEST_OPENGL_ERROR();
    TEST_OPENGL_ERROR();
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0xFF);
    glDepthMask(GL_TRUE);
    glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    render_portals(model_view_matrix, projection_matrix, 0, clip);
    glfwSwapBuffers(window_);
    // glfwPostRedisplay();
}

void Program::render(glm::mat4 const &model_view_matrix,
                     glm::mat4 const &projection_matrix)
{
    set_mat4_uniform("model_view_matrix", model_view_matrix);
    set_mat4_uniform("projection_matrix", projection_matrix);
    set_vec3_uniform("cam_pos", model_view_matrix * glm::vec4(0, 0, 0, 1));
    for (auto obj : scene_->get_objs())
    {
        glBindVertexArray(obj->get_VAO());
        obj->bind_texture(shader_program_);

        set_mat4_uniform("transform", obj->get_transform());

        glDrawArrays(GL_TRIANGLES, 0, obj->get_triangles_number());
        TEST_OPENGL_ERROR();
    }
}

void Program::render_portals(glm::mat4 const &view_mat,
                             glm::mat4 const &proj_mat,
                             unsigned int recursion_level, bool clip)
{
    for (auto portal : scene_->get_portals())
    {
        // Disable color and depth drawing
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        TEST_OPENGL_ERROR();
        glDepthMask(GL_FALSE);
        TEST_OPENGL_ERROR();

        // Disable depth test
        glDisable(GL_DEPTH_TEST);
        TEST_OPENGL_ERROR();

        // Enable stencil test, to prevent drawing outside
        // region of current portal depth
        glEnable(GL_STENCIL_TEST);
        TEST_OPENGL_ERROR();

        // Fail stencil test when inside of outer portal
        // (fail where we should be drawing the inner portal)
        glStencilFunc(GL_NOTEQUAL, recursion_level, 0xFF);
        TEST_OPENGL_ERROR();

        // Increment stencil value on stencil fail
        // (on area of inner portal)
        glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
        TEST_OPENGL_ERROR();

        // Enable (writing into) all stencil bits
        glStencilMask(0xFF);
        TEST_OPENGL_ERROR();

        // Draw portal into stencil buffer
        // portal.draw(viewMat, projMat);
        glBindVertexArray(portal->get_VAO());
        TEST_OPENGL_ERROR();

        set_mat4_uniform("model_view_matrix", view_mat);
        set_mat4_uniform("projection_matrix", proj_mat);
        set_vec3_uniform("cam_pos", view_mat * glm::vec4(0, 0, 0, 1));
        set_mat4_uniform("transform", portal->get_transform());

        glDrawArrays(GL_TRIANGLES, 0, portal->get_triangles_number());

        // Calculate view matrix as if the player was already teleported
        glm::mat4 destView =
            portal_view(view_mat, portal, portal->get_destination());

        // Base case, render inside of inner portal
        if (recursion_level == max_recursion_level_)
        {
            // Enable color and depth drawing
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            TEST_OPENGL_ERROR();
            glDepthMask(GL_TRUE);
            TEST_OPENGL_ERROR();

            // Clear the depth buffer so we don't interfere with stuff
            // outside of this inner portal
            glClear(GL_DEPTH_BUFFER_BIT);
            TEST_OPENGL_ERROR();

            // Enable the depth test
            // So the stuff we render here is rendered correctly
            glEnable(GL_DEPTH_TEST);
            TEST_OPENGL_ERROR();

            // Enable stencil test
            // So we can limit drawing inside of the inner portal
            glEnable(GL_STENCIL_TEST);
            TEST_OPENGL_ERROR();

            // Disable drawing into stencil buffer
            glStencilMask(0x00);
            TEST_OPENGL_ERROR();

            // Draw only where stencil value == recursionLevel + 1
            // which is where we just drew the new portal
            glStencilFunc(GL_EQUAL, recursion_level + 1, 0xFF);
            TEST_OPENGL_ERROR();

            // Draw scene objects with destView, limited to stencil buffer
            // use an edited projection matrix to set the near plane to the
            // portal plane drawNonPortals(destView,
            // portal.clippedProjMat(destView, projMat));

            if (clip)
            {
                render(destView, portal->clippedProjMat(destView, proj_mat));
            }
            else
            {
                render(destView, proj_mat);
            }
        }
        else
        {
            // Recursion case
            // Pass our new view matrix and the clipped projection matrix (see
            // above)
            if (clip)
            {
                render_portals(destView,
                               portal->clippedProjMat(destView, proj_mat),
                               recursion_level + 1, clip);
            }
            else
            {
                render_portals(destView, proj_mat, recursion_level + 1, clip);
            }
        }

        // Disable color and depth drawing
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        TEST_OPENGL_ERROR();
        glDepthMask(GL_FALSE);
        TEST_OPENGL_ERROR();

        // Enable stencil test and stencil drawing
        glEnable(GL_STENCIL_TEST);
        TEST_OPENGL_ERROR();
        glStencilMask(0xFF);
        TEST_OPENGL_ERROR();

        // Fail stencil test when inside of our newly rendered
        // inner portal
        glStencilFunc(GL_NOTEQUAL, recursion_level + 1, 0xFF);
        TEST_OPENGL_ERROR();

        // Decrement stencil value on stencil fail
        // This resets the incremented values to what they were before,
        // eventually ending up with a stencil buffer full of zero's again
        // after the last (outer) step.
        glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);
        TEST_OPENGL_ERROR();

        // Draw portal into stencil buffer
        // portal.draw(viewMat, projMat);
        glBindVertexArray(portal->get_VAO());
        TEST_OPENGL_ERROR();

        set_mat4_uniform("model_view_matrix", view_mat);
        set_mat4_uniform("projection_matrix", proj_mat);
        set_vec3_uniform("cam_pos", view_mat * glm::vec4(0, 0, 0, 1));
        set_mat4_uniform("transform", portal->get_transform());

        glDrawArrays(GL_TRIANGLES, 0, portal->get_triangles_number());
        TEST_OPENGL_ERROR();
    }

    // Disable the stencil test and stencil writing
    glDisable(GL_STENCIL_TEST);
    TEST_OPENGL_ERROR();
    glStencilMask(0x00);
    TEST_OPENGL_ERROR();

    // Disable color writing
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    TEST_OPENGL_ERROR();

    // Enable the depth test, and depth writing.
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();
    glDepthMask(GL_TRUE);
    TEST_OPENGL_ERROR();

    // Make sure we always write the data into the buffer
    glDepthFunc(GL_ALWAYS);
    TEST_OPENGL_ERROR();

    // Clear the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
    TEST_OPENGL_ERROR();

    // Draw portals into depth buffer
    set_mat4_uniform("model_view_matrix", view_mat);
    set_mat4_uniform("projection_matrix", proj_mat);
    set_vec3_uniform("cam_pos", view_mat * glm::vec4(0, 0, 0, 1));
    for (auto tmp : scene_->get_portals())
    {
        glBindVertexArray(tmp->get_VAO());
        TEST_OPENGL_ERROR();

        set_mat4_uniform("transform", tmp->get_transform());

        glDrawArrays(GL_TRIANGLES, 0, tmp->get_triangles_number());
        TEST_OPENGL_ERROR();
    }

    // Reset the depth function to the default
    glDepthFunc(GL_LESS);
    TEST_OPENGL_ERROR();

    // Enable stencil test and disable writing to stencil buffer
    glEnable(GL_STENCIL_TEST);
    TEST_OPENGL_ERROR();
    glStencilMask(0x00);
    TEST_OPENGL_ERROR();

    // Draw at stencil >= recursionlevel
    // which is at the current level or higher (more to the inside)
    // This basically prevents drawing on the outside of this level.
    glStencilFunc(GL_LEQUAL, recursion_level, 0xFF);
    TEST_OPENGL_ERROR();

    // Enable color and depth drawing again
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    TEST_OPENGL_ERROR();
    glDepthMask(GL_TRUE);
    TEST_OPENGL_ERROR();

    // And enable the depth test
    glEnable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();

    // Draw scene objects normally, only at recursionLevel
    render(view_mat, proj_mat);
}
