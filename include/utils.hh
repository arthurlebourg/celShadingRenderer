#pragma once

#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include <btBulletDynamicsCommon.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

void test_opengl_error(std::string func, std::string file, int line);

#define TEST_OPENGL_ERROR()                                                    \
    do                                                                         \
    {                                                                          \
        test_opengl_error(__func__, __FILE__, __LINE__);                       \
    } while (0)

void load_obj(const char *filename, std::vector<glm::vec3> &vertices,
              std::vector<glm::vec2> &uv, std::vector<glm::vec3> &normals);

std::string read_file(const std::string &filename);

btVector3 glmToBullet(const glm::vec3 &v);

btMatrix3x3 glmToBullet(const glm::mat3 &m);

btTransform glmToBullet(const glm::mat4 &m);

glm::vec3 get_vector(const glm::vec3 point_src, const glm::vec3 point_dest,
                     float angle);
glm::vec3 find_normale(const glm::vec3 point_src, const glm::vec3 point_dest1,
                       const glm::vec3 point_dest2, float angle);

float reset_angle(float angle_to_be_set, const float angle);

bool is_sameSign(const float f1, const float f2);
