#ifndef WORLD_H
#define WORLD_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include "Car.hpp"
#include "Objeto.hpp"
#include "GLDebugDrawer.hpp"
using namespace glm;

class World{

    private:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        GLDebugDrawer* debug;
    public:
        World();
        void addObject(Objeto *obj);
        void debugDrawer(GLDebugDrawer* debug);
        void debugWorld();
};

#endif