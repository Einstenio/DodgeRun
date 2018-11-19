#include "Objeto.hpp"

Objeto::Objeto(){

}

Objeto::Objeto(char* PATH, btVector3 posicionInicial, btScalar masa, bool estatico){
    if (estatico) this->objShape = load_mesh_point_static(PATH);
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
