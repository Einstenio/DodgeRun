//Librerias

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <iostream>
#include "gl_utils.h"
#include "tools.h"
#include "malla.h"
#include "Objeto.h"
#include "GLDebugDrawer.hpp"

//Constantes

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

//Visualización en pantalla

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

//Declara Métodos

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void init();

//Opciones Sobre la Cámara

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  45.0f;

// Timing FPS

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){

	init();

	//Creación de Shaders

	GLuint shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	glUseProgram (shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
	glUseProgram (shader_programme);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
	int model_mat_location = glGetUniformLocation (shader_programme, "model");

	//Creación del Mundo Físico

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0)); //Gravedad

	//Depuración

	GLDebugDrawer* debug = new GLDebugDrawer();
	debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe );
	debug->setView(&view);
	debug->setProj(&projection);
	dynamicsWorld->setDebugDrawer(debug);

	//Creación de Figuras

	malla *box = new malla((char*)"mallas/box/box.obj");
	malla *highway = new malla((char*)"mallas/highway/highway.obj");
	malla *car = new malla((char*)"mallas/car/car.obj");
	malla *rock = new malla((char*)"mallas/rock/rock.obj");

	//Creación de Cuerpos con Clase Objeto

	/*
	Objeto *automovil = new Objeto((char*)"mallas/car/car.obj", btVector3(0, 15, -1), btScalar(0.));
	dynamicsWorld->addRigidBody(automovil->objBody);

	Objeto *piedra00 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(15, -5, -40), btScalar(0.));
	dynamicsWorld->addRigidBody(piedra00->objBody);

	Objeto *caja00 = new Objeto((char*)"mallas/box/box.obj", btVector3(0, 20, -20), btScalar(10.));
	dynamicsWorld->addRigidBody(caja00->objBody);

	Objeto *caja01 = new Objeto((char*)"mallas/box/box.obj", btVector3(-3, 30, -40), btScalar(10.));
	dynamicsWorld->addRigidBody(caja01->objBody);

	Objeto *caja02 = new Objeto((char*)"mallas/box/box.obj", btVector3(5, 10, -60), btScalar(10.));
	dynamicsWorld->addRigidBody(caja02->objBody);

	Objeto *caja03 = new Objeto((char*)"mallas/box/box.obj", btVector3(-3, 40, -80), btScalar(10.));
	dynamicsWorld->addRigidBody(caja03->objBody);

	Objeto *caja04 = new Objeto((char*)"mallas/box/box.obj", btVector3(0, 80, -100), btScalar(10.));
	dynamicsWorld->addRigidBody(caja04->objBody);

	Objeto *caja05 = new Objeto((char*)"mallas/box/box.obj", btVector3(1, 70, -120), btScalar(10.));
	dynamicsWorld->addRigidBody(caja04->objBody);
	*/

	Objeto *carretera = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -7, -115), btScalar(0.));
	dynamicsWorld->addRigidBody(carretera->objBody);

	//Creación de Cuerpos sin Clase Objeto

	btCollisionShape* caja01Shape = new btBoxShape(btVector3(1, 1, 1));
	btTransform caja01Transform;
	caja01Transform.setIdentity();
	caja01Transform.setOrigin(btVector3(0, 20, -20));
	btScalar caja01Mass(10.); // Masa
	bool isDynamiccaja01 = (caja01Mass != 0.f);
	btVector3 localInertiacaja01(1, 0, 0);
	if (isDynamiccaja01) caja01Shape->calculateLocalInertia(caja01Mass, localInertiacaja01);
	btDefaultMotionState* caja01MotionState = new btDefaultMotionState(caja01Transform);
	btRigidBody::btRigidBodyConstructionInfo caja01RbInfo(caja01Mass, caja01MotionState, caja01Shape, localInertiacaja01);
	btRigidBody* bodycaja01 = new btRigidBody(caja01RbInfo);
	bodycaja01->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycaja01);

	btCollisionShape* caja02Shape = new btBoxShape(btVector3(1, 1, 1));
	btTransform caja02Transform;
	caja02Transform.setIdentity();
	caja02Transform.setOrigin(btVector3(-3, 30, -40));
	btScalar caja02Mass(10.);
	bool isDynamiccaja02 = (caja02Mass != 0.f);
	btVector3 localInertiacaja02(1, 0, 0);
	if (isDynamiccaja02) caja02Shape->calculateLocalInertia(caja02Mass, localInertiacaja02);
	btDefaultMotionState* caja02MotionState = new btDefaultMotionState(caja02Transform);
	btRigidBody::btRigidBodyConstructionInfo caja02RbInfo(caja02Mass, caja02MotionState, caja02Shape, localInertiacaja02);
	btRigidBody* bodycaja02 = new btRigidBody(caja02RbInfo);
	bodycaja02->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycaja02);

	btCollisionShape* caja03Shape = new btBoxShape(btVector3(1, 1, 1));
	btTransform caja03Transform;
	caja03Transform.setIdentity();
	caja03Transform.setOrigin(btVector3(5, 10, -60));
	btScalar caja03Mass(10.);
	bool isDynamiccaja03 = (caja03Mass != 0.f);
	btVector3 localInertiacaja03(1, 0, 0);
	if (isDynamiccaja03) caja03Shape->calculateLocalInertia(caja03Mass, localInertiacaja03);
	btDefaultMotionState* caja03MotionState = new btDefaultMotionState(caja03Transform);
	btRigidBody::btRigidBodyConstructionInfo caja03RbInfo(caja03Mass, caja03MotionState, caja03Shape, localInertiacaja03);
	btRigidBody* bodycaja03 = new btRigidBody(caja03RbInfo);
	bodycaja03->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycaja03);

	btCollisionShape* caja04Shape = new btBoxShape(btVector3(1, 1, 1));
	btTransform caja04Transform;
	caja04Transform.setIdentity();
	caja04Transform.setOrigin(btVector3(-3, 40, -80));
	btScalar caja04Mass(10.);
	bool isDynamiccaja04 = (caja04Mass != 0.f);
	btVector3 localInertiacaja04(1, 0, 0);
	if (isDynamiccaja04) caja04Shape->calculateLocalInertia(caja04Mass, localInertiacaja04);
	btDefaultMotionState* caja04MotionState = new btDefaultMotionState(caja04Transform);
	btRigidBody::btRigidBodyConstructionInfo caja04RbInfo(caja04Mass, caja04MotionState, caja04Shape, localInertiacaja04);
	btRigidBody* bodycaja04 = new btRigidBody(caja04RbInfo);
	bodycaja04->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycaja04);

	btCollisionShape* caja05Shape = new btBoxShape(btVector3(1, 1, 1));
	btTransform caja05Transform;
	caja05Transform.setIdentity();
	caja05Transform.setOrigin(btVector3(0, 80, -100));
	btScalar caja05Mass(10.);
	bool isDynamiccaja05 = (caja05Mass != 0.f);
	btVector3 localInertiacaja05(1, 0, 0);
	if (isDynamiccaja05) caja05Shape->calculateLocalInertia(caja05Mass, localInertiacaja05);
	btDefaultMotionState* caja05MotionState = new btDefaultMotionState(caja05Transform);
	btRigidBody::btRigidBodyConstructionInfo caja05RbInfo(caja05Mass, caja05MotionState, caja05Shape, localInertiacaja05);
	btRigidBody* bodycaja05 = new btRigidBody(caja05RbInfo);
	bodycaja05->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycaja05);

	btCollisionShape* caja06Shape = new btBoxShape(btVector3(1, 1, 1));
	btTransform caja06Transform;
	caja06Transform.setIdentity();
	caja06Transform.setOrigin(btVector3(1, 70, -120));
	btScalar caja06Mass(10.);
	bool isDynamiccaja06 = (caja06Mass != 0.f);
	btVector3 localInertiacaja06(1, 0, 0);
	if (isDynamiccaja06) caja06Shape->calculateLocalInertia(caja06Mass, localInertiacaja06);
	btDefaultMotionState* caja06MotionState = new btDefaultMotionState(caja06Transform);
	btRigidBody::btRigidBodyConstructionInfo caja06RbInfo(caja06Mass, caja06MotionState, caja06Shape, localInertiacaja06);
	btRigidBody* bodycaja06 = new btRigidBody(caja06RbInfo);
	bodycaja06->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycaja06);

	btCollisionShape* carShape;
	carShape = load_mesh_point((char*)"mallas/car/car.obj");
	btTransform carTransform;
	carTransform.setIdentity();
	carTransform.setOrigin(btVector3(0, -6, -1));
	btScalar carMass(0.);
	bool isDynamiccar = (carMass != 1.0f);
	btVector3 localInertiacar(0, 0, 0);
	if (isDynamiccar) carShape->calculateLocalInertia(carMass, localInertiacar);
	btDefaultMotionState* carMotionState = new btDefaultMotionState(carTransform);
	btRigidBody::btRigidBodyConstructionInfo crInfo(carMass, carMotionState, carShape, localInertiacar);
	btRigidBody* bodycar = new btRigidBody(crInfo);
	bodycar->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodycar);

	btCollisionShape* piedraShape;
	piedraShape = load_mesh_point((char*)"mallas/rock/rock.obj");
	btTransform piedraTransform;
	piedraTransform.setIdentity();
	piedraTransform.setOrigin(btVector3(15, -5, -40));
	btScalar piedraMass(0.);
	bool isDynamicpiedra = (piedraMass != 1.0f);
	btVector3 localInertiapiedra(0, 0, 0);
	if (isDynamicpiedra) piedraShape->calculateLocalInertia(piedraMass, localInertiapiedra);
	btDefaultMotionState* piedraMotionState = new btDefaultMotionState(piedraTransform);
	btRigidBody::btRigidBodyConstructionInfo piedraInfo(piedraMass, piedraMotionState, piedraShape, localInertiapiedra);
	btRigidBody* bodypiedra = new btRigidBody(piedraInfo);
	bodypiedra->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(bodypiedra);

	//Otros

    glm::mat4 aux;

    while (!glfwWindowShouldClose(g_window)){

        // Variables Previas a Ejecución

		dynamicsWorld->stepSimulation(1.f / 60.f, 10);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(g_window);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram (shader_programme);
        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

        btTransform trans;

		//Visualización de Objetos en la pantalla

		carretera->objBody->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(&aux[0][0]);
		highway->setModelMatrix(aux);
		highway->draw(model_mat_location);		

		bodycar->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(&aux[0][0]);
		car->setModelMatrix(aux);
		car->draw(model_mat_location);

		bodypiedra->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(&aux[0][0]);
		rock->setModelMatrix(aux);
		rock->draw(model_mat_location);

		bodycaja01->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        box->setModelMatrix(aux);
        box->draw(model_mat_location);

        bodycaja02->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        box->setModelMatrix(aux);
        box->draw(model_mat_location);

		bodycaja03->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        box->setModelMatrix(aux);
        box->draw(model_mat_location);

        bodycaja04->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        box->setModelMatrix(aux);
        box->draw(model_mat_location);

        bodycaja05->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        box->setModelMatrix(aux);
        box->draw(model_mat_location);

        bodycaja06->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        box->setModelMatrix(aux);
        box->draw(model_mat_location);

		//Depuración

		debug->setView(&view);
		debug->setProj(&projection);
		dynamicsWorld->debugDrawWorld();
		debug->drawLines();

		//Otros

        glfwSwapBuffers(g_window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;

}

//Métodos

void init(){
	restart_gl_log ();
	start_gl ();
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);
	glEnable (GL_CULL_FACE);
	glCullFace (GL_BACK);
	glFrontFace (GL_CCW);
	glClearColor (0.2, 0.2, 0.2, 1.0);
	glViewport (0, 0, g_gl_width, g_gl_height);
	glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(g_window, mouse_callback);
	glfwSetScrollCallback(g_window, scroll_callback);
	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 50. * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
