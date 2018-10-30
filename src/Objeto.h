using namespace std;
#include "tools.h"
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
        Objeto(char* PATH, btVector3 posicionInicial, btScalar masa);
        btRigidBody* objBody;
};
