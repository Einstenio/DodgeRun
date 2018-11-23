#ifndef OBJETO_H
#define OBJETO_H

#include "tools.hpp"
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "malla.h"

using namespace glm;

class Objeto{
    private:
        btCollisionShape* objShape;
        btTransform objTransform;
        btScalar objMass;
        bool isDynamicObj;
        btVector3 localInertiaObj;
        btDefaultMotionState* objMotionState;

    public:
        Objeto(char* PATH, btVector3 posicionInicial, btScalar masa);
        Objeto(char* PATH, btVector3 posicionInicial, btScalar masa, btDynamicsWorld* dynamicsWorld);
        void showObject(btTransform trans, mat4 aux, malla *model, int location);
        btRigidBody* objBody;
};

#endif
