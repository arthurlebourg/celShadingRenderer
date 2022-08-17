#pragma once
#include <memory>

#include "object.hh"
#include "player.hh"

class Scene
{
public:
    Scene(const glm::vec3 light);

    void add_object(const std::shared_ptr<Object> obj);

    void add_player(std::shared_ptr<Player> player);

    std::shared_ptr<Player> get_player();

    std::vector<std::shared_ptr<Object>> get_objs();

    glm::vec3 get_light();

    btDiscreteDynamicsWorld *get_dynamic_world();

private:
    std::shared_ptr<Player> player_;

    glm::vec3 light_;

    std::vector<std::shared_ptr<Object>> objects_;

    btDiscreteDynamicsWorld *dynamicsWorld_;
};
