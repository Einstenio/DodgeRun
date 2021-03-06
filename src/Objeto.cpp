#include "Objeto.hpp"

Objeto::Objeto(char* PATH, btVector3 posicionInicial, btScalar masa){
    if (masa == (btScalar)(0.)) this->objShape = load_mesh_point_static(PATH);
    else this->objShape = load_mesh_point(PATH);    
    this->objShape = load_mesh_point(PATH);
    this->objTransform.setIdentity();
    this->objTransform.setOrigin(posicionInicial);
    this->objMass = masa;
    this->isDynamicObj = (this->objMass != 0.f);
    this->localInertiaObj = btVector3(1,0,0);
    if (this->isDynamicObj) (this->objShape)->calculateLocalInertia(this->objMass, this->localInertiaObj);
    this->objMotionState = new btDefaultMotionState(this->objTransform);
    btRigidBody::btRigidBodyConstructionInfo objInfo(this->objMass, this->objMotionState, this->objShape, this->localInertiaObj);
    objBody = new btRigidBody(objInfo);
    objBody->setActivationState(DISABLE_DEACTIVATION);
}

Objeto::Objeto(char* PATH, btVector3 posicionInicial, btScalar masa, btDynamicsWorld* dynamicsWorld){
    if (masa == (btScalar)(0.)) this->objShape = load_mesh_point_static(PATH);
    else this->objShape = load_mesh_point(PATH);    
    this->objShape = load_mesh_point(PATH);
    this->objTransform.setIdentity();
    this->objTransform.setOrigin(posicionInicial);
    this->objMass = masa;
    this->isDynamicObj = (this->objMass != 0.f);
    this->localInertiaObj = btVector3(1,0,0);
    if (this->isDynamicObj) (this->objShape)->calculateLocalInertia(this->objMass, this->localInertiaObj);
    this->objMotionState = new btDefaultMotionState(this->objTransform);
    btRigidBody::btRigidBodyConstructionInfo objInfo(this->objMass, this->objMotionState, this->objShape, this->localInertiaObj);
    objBody = new btRigidBody(objInfo);
    objBody->setActivationState(DISABLE_DEACTIVATION);
    dynamicsWorld->addRigidBody(objBody);
}

void Objeto::showObject(btTransform trans, mat4 aux, malla *model, int location){
    this->objBody->getMotionState()->getWorldTransform(trans);
    trans.getOpenGLMatrix(&aux[0][0]);
    model->setModelMatrix(aux);
    model->draw(location);
}

void Objeto::setVelocity(btVector3 speed){
    objBody->setLinearVelocity(speed);
}
