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
        Car(btVector3 posicionInicial, btDynamicsWorld* dynamicsWorld);
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
