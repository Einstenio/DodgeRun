#include "Car.hpp"

char* PATH = (char*)"mallas/car/car.obj";
btScalar MASS = btScalar(1300.);

Car::Car(btVector3 posicionInicial, btDynamicsWorld* dynamicsWorld) : Objeto(PATH, posicionInicial, MASS, dynamicsWorld){
	this->heart = 3;
	this->gasoline = 100;
	this->isAlive = true;
	this->speed = 10;
}

void Car::setHeart(int heart){
	this->heart = heart;
}

int Car::getHeart(){
	return heart;
}

void Car::setGasoline(int gasoline){
	this->gasoline = gasoline;
}

int Car::getGasoline(){
	return gasoline;
}

void Car::setIsAlive(bool isAlive){
	this->isAlive = isAlive;
}

bool Car::getIsAlive(){
	return isAlive;
}

void Car::setSpeed(float speed){
	this->speed = speed;
}

float Car::getSpeed(){
	return speed;
}