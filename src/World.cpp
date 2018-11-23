#include "World.hpp"

World::World(){
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

void World::addObject(Objeto *obj){
    dynamicsWorld->addRigidBody(obj->objBody);
}

void World::addObject(btRigidBody *obj){
    dynamicsWorld->addRigidBody(obj);
}


btDiscreteDynamicsWorld* World::getDynamicsWorld(){
    return dynamicsWorld;
}

