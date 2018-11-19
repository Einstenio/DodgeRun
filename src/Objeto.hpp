#ifndef OBJETO_H
#define OBJETO_H

using namespace std;
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
        Objeto();
        Objeto(char* PATH, btVector3 posicionInicial, btScalar masa, bool estatico);
        btRigidBody* objBody;
};

#endif