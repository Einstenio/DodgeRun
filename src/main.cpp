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

	//Variables Previas para Ejecucación

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

	malla *ball = new malla((char*)"mallas/box/box.obj");
	malla *piso = new malla((char*)"mallas/highway/highway.obj");

	//Creación de Cuerpos

	//Pelota

	btCollisionShape* ballShape = new btBoxShape(btVector3(1, 1, 1)); // Radio de Esfera
	btTransform ballTransform;
	ballTransform.setIdentity();
	ballTransform.setOrigin(btVector3(0, 20, 1)); // Posicion incial
	btScalar ballMass(10.); // Masa
	bool isDynamicBall = (ballMass != 0.f);
	btVector3 localInertiaBall(1, 0, 0);
	if (isDynamicBall) ballShape->calculateLocalInertia(ballMass, localInertiaBall);

	btDefaultMotionState* ballMotionState = new btDefaultMotionState(ballTransform);
	btRigidBody::btRigidBodyConstructionInfo ballRbInfo(ballMass, ballMotionState, ballShape, localInertiaBall);
	btRigidBody* bodyBall = new btRigidBody(ballRbInfo);
  	bodyBall->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(bodyBall);

	//Piso

	btCollisionShape* groundShape; // Tamaño de Piso
	groundShape = load_mesh_point((char*)"mallas/highway/highway.obj");
	btTransform pisoTransform;
	pisoTransform.setIdentity();
	pisoTransform.setOrigin(btVector3(0, -3, -2)); // Posicion incial
	btScalar pisoMass(0.); // Masa
	bool isDynamicPiso = (pisoMass != 0.f);
	btVector3 localInertiaPiso(0, 0, 0);
	if (isDynamicPiso) groundShape->calculateLocalInertia(pisoMass, localInertiaPiso);

	btDefaultMotionState* pisoMotionState = new btDefaultMotionState(pisoTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(pisoMass, pisoMotionState, groundShape, localInertiaPiso);
	btRigidBody* bodyPiso = new btRigidBody(rbInfo);
	bodyPiso->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(bodyPiso);

	//Extras

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

		//Pelota

        bodyBall->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        ball->setModelMatrix(aux);
        ball->draw(model_mat_location);

		//Piso

		bodyPiso->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(&aux[0][0]);
		piso->setModelMatrix(aux);
		piso->draw(model_mat_location);

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

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    float cameraSpeed = 2.5 * deltaTime;
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
