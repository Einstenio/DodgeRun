#ifndef OBJETO_H
#define OBJETO_H

#include "tools.hpp"
#include <bullet/btBulletDynamicsCommon.h>

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
        void showObject(btTransform trans);
        btRigidBody* objBody;
};

#endif