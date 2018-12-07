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
#include "World.hpp"
#include "Objeto.hpp"
#include "Car.hpp"
#include "GLDebugDrawer.hpp"
using namespace glm;

//Constantes

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

//Visualización en pantalla

int g_gl_width = 1280;
int g_gl_height = 720;
GLFWwindow* g_window = NULL;

//Declara Métodos

void processInput(GLFWwindow *window);

//Opciones Sobre la Cámara

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f,10.0f, 0.0f);

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

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	glUseProgram (shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
	glUseProgram (shader_programme);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
	int model_mat_location = glGetUniformLocation (shader_programme, "model");

	//Creación del Mundo Físico

	World *mundo = new World();

	//Creación de Figuras

	malla *box = new malla((char*)"mallas/box/box.obj", shader_programme);
    box->load_texture("mallas/box/box.jpg");

	malla *highway = new malla((char*)"mallas/highway/highway.obj", shader_programme);
	malla *car = new malla((char*)"mallas/car/car.obj", shader_programme);
	malla *rock = new malla((char*)"mallas/rock/rock.obj", shader_programme);

	//Creación de Cuerpos con Clase Objeto

	Objeto *carretera = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -5, -100), btScalar(0.), mundo->getDynamicsWorld());
	Car *automovil = new Car(btVector3(0, 15, -1), mundo->getDynamicsWorld());
	Objeto *piedra00 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(15, -5, -40), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *caja00 = new Objeto((char*)"mallas/box/box.obj", btVector3(0, 20, -20), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja01 = new Objeto((char*)"mallas/box/box.obj", btVector3(-3, 30, -40), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja02 = new Objeto((char*)"mallas/box/box.obj", btVector3(5, 10, -60), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja03 = new Objeto((char*)"mallas/box/box.obj", btVector3(-3, 40, -80), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja04 = new Objeto((char*)"mallas/box/box.obj", btVector3(0, 80, -100), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja05 = new Objeto((char*)"mallas/box/box.obj", btVector3(1, 70, -120), btScalar(10.), mundo->getDynamicsWorld());

	//Otros

    mat4 aux;

    while (!glfwWindowShouldClose(g_window)){

        // Variables Previas a Ejecución

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
		if (deltaTime < 0.0166) continue;
        lastFrame = currentFrame;
        processInput(g_window);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram (shader_programme);
        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
		mundo->getDynamicsWorld()->stepSimulation(1/60.f, 10);

        btTransform trans;

		//Visualización de Objetos en la pantalla

		carretera->showObject(trans, aux, highway, model_mat_location);
		automovil->showObject(trans, aux, car, model_mat_location);
		piedra00->showObject(trans, aux, rock, model_mat_location);
		caja00->showObject(trans, aux, box, model_mat_location);
		caja01->showObject(trans, aux, box, model_mat_location);
		caja02->showObject(trans, aux, box, model_mat_location);
		caja03->showObject(trans, aux, box, model_mat_location);
		caja04->showObject(trans, aux, box, model_mat_location);
		caja05->showObject(trans, aux, box, model_mat_location);

		//Mover Auto

		if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS) automovil->setVelocity(btVector3(0,0,-5));
		if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS) automovil->setVelocity(btVector3(0,0,5));
		if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS) automovil->setVelocity(btVector3(-5,0,0));
		if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS) automovil->setVelocity(btVector3(5,0,0));

		//Otros

        glfwSwapBuffers(g_window);
        glfwPollEvents();

    }

    glfwTerminate();
	
    return 0;

}

//Métodos

void processInput(GLFWwindow *window){
	float cameraSpeed = 100. * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
