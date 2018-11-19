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
#include "tools.hpp"
#include "malla.h"
#include "Objeto.hpp"
#include "GLDebugDrawer.hpp"

//Constantes

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

//Visualización en pantalla

int g_gl_width = 1280;
int g_gl_height = 720;
GLFWwindow* g_window = NULL;

//Declara Métodos

void mouse_callback(GLFWwindow* window, double xpos, double ypos); //Usar mouse -BORRAR-
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

	init();
	
	glfwSetCursorPosCallback(g_window, mouse_callback); //Usar mouse -BORRAR-

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
	dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

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

	Objeto *automovil = new Objeto((char*)"mallas/car/car.obj", btVector3(0, 15, -1), btScalar(1300.), false);
	dynamicsWorld->addRigidBody(automovil->objBody);

	Objeto *piedra00 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(15, -5, -40), btScalar(0.), true);
	dynamicsWorld->addRigidBody(piedra00->objBody);

	Objeto *caja00 = new Objeto((char*)"mallas/box/box.obj", btVector3(0, 20, -20), btScalar(10.), false);
	dynamicsWorld->addRigidBody(caja00->objBody);

	Objeto *caja01 = new Objeto((char*)"mallas/box/box.obj", btVector3(-3, 30, -40), btScalar(10.), false);
	dynamicsWorld->addRigidBody(caja01->objBody);

	Objeto *caja02 = new Objeto((char*)"mallas/box/box.obj", btVector3(5, 10, -60), btScalar(10.), false);
	dynamicsWorld->addRigidBody(caja02->objBody);

	Objeto *caja03 = new Objeto((char*)"mallas/box/box.obj", btVector3(-3, 40, -80), btScalar(10.), false);
	dynamicsWorld->addRigidBody(caja03->objBody);

	Objeto *caja04 = new Objeto((char*)"mallas/box/box.obj", btVector3(0, 80, -100), btScalar(10.), false);
	dynamicsWorld->addRigidBody(caja04->objBody);

	Objeto *caja05 = new Objeto((char*)"mallas/box/box.obj", btVector3(1, 70, -120), btScalar(10.), false);
	dynamicsWorld->addRigidBody(caja04->objBody);

	Objeto *carretera = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -7, -115), btScalar(0.), true);
	dynamicsWorld->addRigidBody(carretera->objBody);

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
	float cameraSpeed = 50. * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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
