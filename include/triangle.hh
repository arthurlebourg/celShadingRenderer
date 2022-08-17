#pragma once

#include <btBulletDynamicsCommon.h>

#include "utils.hh"

class Triangle
{
public:
    Triangle(const glm::vec3 x, const glm::vec3 y, const glm::vec3 z);

private:
    glm::vec3 x;
    glm::vec3 y;
    glm::vec3 z;
}
