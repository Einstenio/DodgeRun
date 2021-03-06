#ifndef WORLD_H
#define WORLD_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include "Objeto.hpp"
#include "Car.hpp"
using namespace glm;

class World{

    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
    public:
        World();
        void addObject(Objeto *obj);
        void addObject(btRigidBody *obj);
        btDiscreteDynamicsWorld* getDynamicsWorld();
};

#endif
