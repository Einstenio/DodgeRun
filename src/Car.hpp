#ifndef CAR_H
#define CAR_H
#include "Objeto.hpp"

class Car : public Objeto{
    private:
        int heart;
        int gasoline;
        float speed;
        bool isAlive;

    public:
        Car(char* PATH, btVector3 posicionInicial, btScalar masa, btDynamicsWorld* dynamicsWorld);
        void setHeart(int heart);
        int getHeart();
        void setGasoline(int gasoline);
        int getGasoline();
        void setIsAlive(bool isAlive);
        bool getIsAlive();
        void setSpeed(float speed);
        float getSpeed();
};

#endif
