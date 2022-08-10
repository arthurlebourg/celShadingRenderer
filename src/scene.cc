#include "scene.hh"

#include <cmath>

Scene::Scene(const glm::vec3 light)
    : light_(light)
{
    /// collision configuration contains default setup for memory, collision
    /// setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration *collisionConfiguration =
        new btDefaultCollisionConfiguration();

    /// use the default collision dispatcher. For parallel processing you can
    /// use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher *dispatcher =
        new btCollisionDispatcher(collisionConfiguration);

    /// btDbvtBroadphase is a good general purpose broadphase. You can also try
    /// out btAxis3Sweep.
    btBroadphaseInterface *overlappingPairCache = new btDbvtBroadphase();

    /// the default constraint solver. For parallel processing you can use a
    /// different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver *solver =
        new btSequentialImpulseConstraintSolver;

    dynamicsWorld_ = new btDiscreteDynamicsWorld(
        dispatcher, overlappingPairCache, solver, collisionConfiguration);

    dynamicsWorld_->setGravity(btVector3(0, -10, 0));
}

void Scene::add_object(const std::shared_ptr<Object> obj)
{
    objects_.push_back(obj);
    dynamicsWorld_->addRigidBody(obj->get_body());
}

void Scene::add_portals(const std::shared_ptr<Portal> portals)
{
    portals_.push_back(portals);
}

std::vector<std::shared_ptr<Portal>> Scene::get_portals()
{
    return portals_;
}

void Scene::add_player(std::shared_ptr<Player> player)
{
    player_ = player;
    dynamicsWorld_->addRigidBody(player->get_body());
}

std::shared_ptr<Player> Scene::get_player()
{
    return player_;
}

std::vector<std::shared_ptr<Object>> Scene::get_objs()
{
    return objects_;
}

glm::vec3 Scene::get_light()
{
    return light_;
}

btDiscreteDynamicsWorld *Scene::get_dynamic_world()
{
    return dynamicsWorld_;
}
