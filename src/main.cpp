//Librerias
#include <time.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
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
#include "stb_image.h"

using namespace glm;

//Constantes

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

#define SKYBOX_VERTEX_SHADER_FILE "shaders/sky_vert.glsl"
#define SKYBOX_FRAGMENT_SHADER_FILE "shaders/sky_frag.glsl"

#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444


//Visualización en pantalla

int g_gl_width = 1280;
int g_gl_height = 720;
GLFWwindow* g_window = NULL;
int lugar=0;

//Declara Métodos

bool processInput(GLFWwindow *window, Car *automovil, bool avanza);

bool load_cube_map_side( GLuint texture, GLenum side_target, const char *file_name ) {
	
	glBindTexture( GL_TEXTURE_CUBE_MAP, texture );

	int x, y, n;
	int force_channels = 4;
	unsigned char *image_data = stbi_load( file_name, &x, &y, &n, force_channels );
	if ( !image_data ) {
		fprintf( stderr, "ERROR: could not load %s\n", file_name );
		return false;
	}
	// non-power-of-2 dimensions check
	if ( ( x & ( x - 1 ) ) != 0 || ( y & ( y - 1 ) ) != 0 ) {
		fprintf( stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name );
	}

	// copy image data into 'target' side of cube map
	glTexImage2D( side_target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data );
	free( image_data );
	return true;
}

//Opciones Sobre la Cámara

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f,10.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  85.0f;

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

	GLuint skybox_shader = create_programme_from_files ( SKYBOX_VERTEX_SHADER_FILE, SKYBOX_FRAGMENT_SHADER_FILE);
	glUseProgram (skybox_shader);
	int view_skybox = glGetUniformLocation (skybox_shader, "view");
	int proj_skybox = glGetUniformLocation (skybox_shader, "proj");
	glUniformMatrix4fv (view_skybox, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv (proj_skybox, 1, GL_FALSE, &projection[0][0]);


	//Creación del Mundo Físico

	World *mundo = new World();

	//Creación de Figuras

	malla *box = new malla((char*)"mallas/box/box.obj", shader_programme);
    box->load_texture("mallas/box/woodbox.jpg");

	malla *highway = new malla((char*)"mallas/highway/highway.obj", shader_programme);
	highway->load_texture("mallas/highway/highway.jpg");

	malla *arena = new malla((char*)"mallas/arena/arena.obj", shader_programme);
	arena->load_texture("mallas/arena/arena.jpg");

	malla *car = new malla((char*)"mallas/car/car.obj", shader_programme);

	malla *rock = new malla((char*)"mallas/rock/rock.obj", shader_programme);
	

	//Creación de Cuerpos con Clase Objeto

	Objeto *carretera0 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -5, -100), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *carretera1 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -5, -349.3), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *carretera2 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -5, -598.6), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *carretera3 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(0, -5, -847.9), btScalar(0.), mundo->getDynamicsWorld());
	Car *automovil = new Car(btVector3(0, -3, -1), mundo->getDynamicsWorld());

	Objeto *piedra00 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(0, -3, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra01 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(6, -3, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra02 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(-6, -3, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra03 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(0, 0, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra04 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(6, 0, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra05 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(-6, 0, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra06 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(-3, -1.5, -715), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *piedra07 = new Objeto((char*)"mallas/rock/rock.obj", btVector3(3, -1.5, -715), btScalar(0.), mundo->getDynamicsWorld());


  	Objeto *arena00 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(-23, -3, -100), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena01 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(-23, -3, -349.3), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena02 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(-23, -3, -598.6), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena03 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(23, -3, -100), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena04 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(23, -3, -349.3), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena05 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(23, -3, -598.6), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena06 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(-23, -3, -847.9), btScalar(0.), mundo->getDynamicsWorld());
	Objeto *arena07 = new Objeto((char*)"mallas/highway/highway.obj", btVector3(23, -3, -847.9), btScalar(0.), mundo->getDynamicsWorld());

	srand(time(NULL));
	//(xyz)
	
	//Objeto *caja01 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 10, -40), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja02 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 20, -60), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja03 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 40, -80), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja04 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 60, -100), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja05 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 80, -440), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja06 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 100, -120), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja07 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 120, -140), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja08 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 140, -160), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja09 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 160, -180), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja10 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 180, -200), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja11 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 200, -220), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja12 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 220, -240), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja13 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 240, -260), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja14 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 260, -280), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja15 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 280, -300), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja16 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 300, -320), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja17 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 320, -340), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja18 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 340, -360), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja19 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 360, -380), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja20 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 380, -400), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja21 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 400, -420), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja22 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 420, -440), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja23 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 440, -460), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja24 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 460, -480), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja25 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 480, -500), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja26 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 500, -520), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja27 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 520, -540), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja28 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 540, -560), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja29 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 560, -580), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja30 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 580, -600), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja31 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 600, -620), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja32 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 620, -640), btScalar(10.), mundo->getDynamicsWorld());
	Objeto *caja33 = new Objeto((char*)"mallas/box/box.obj", btVector3(rand() % (16)-8, 640, -660), btScalar(10.), mundo->getDynamicsWorld());

	
//Skybox

	float points[] = {
		-10.0f, 10.0f,	-10.0f, -10.0f, -10.0f, -10.0f, 10.0f,	-10.0f, -10.0f,
		10.0f,	-10.0f, -10.0f, 10.0f,	10.0f,	-10.0f, -10.0f, 10.0f,	-10.0f,

		-10.0f, -10.0f, 10.0f,	-10.0f, -10.0f, -10.0f, -10.0f, 10.0f,	-10.0f,
		-10.0f, 10.0f,	-10.0f, -10.0f, 10.0f,	10.0f,	-10.0f, -10.0f, 10.0f,

		10.0f,	-10.0f, -10.0f, 10.0f,	-10.0f, 10.0f,	10.0f,	10.0f,	10.0f,
		10.0f,	10.0f,	10.0f,	10.0f,	10.0f,	-10.0f, 10.0f,	-10.0f, -10.0f,

		-10.0f, -10.0f, 10.0f,	-10.0f, 10.0f,	10.0f,	10.0f,	10.0f,	10.0f,
		10.0f,	10.0f,	10.0f,	10.0f,	-10.0f, 10.0f,	-10.0f, -10.0f, 10.0f,

		-10.0f, 10.0f,	-10.0f, 10.0f,	10.0f,	-10.0f, 10.0f,	10.0f,	10.0f,
		10.0f,	10.0f,	10.0f,	-10.0f, 10.0f,	10.0f,	-10.0f, 10.0f,	-10.0f,

		-10.0f, -10.0f, -10.0f, -10.0f, -10.0f, 10.0f,	10.0f,	-10.0f, -10.0f,
		10.0f,	-10.0f, -10.0f, -10.0f, -10.0f, 10.0f,	10.0f,	-10.0f, 10.0f
	};
	GLuint vbosky;
	glGenBuffers( 1, &vbosky );
	glBindBuffer( GL_ARRAY_BUFFER, vbosky );
	glBufferData( GL_ARRAY_BUFFER, 3 * 36 * sizeof( GLfloat ), &points, GL_STATIC_DRAW );

	GLuint vaosky;
	glGenVertexArrays( 1, &vaosky );
	glBindVertexArray( vaosky );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vbosky );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	GLuint cube_map_texture;
	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &cube_map_texture );

	load_cube_map_side( cube_map_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "textures/arrakisday_bk.tga" );
	load_cube_map_side( cube_map_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "textures/arrakisday_ft.tga"  );
	load_cube_map_side( cube_map_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "textures/up.tga" ) ;
	load_cube_map_side( cube_map_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "textures/arrakisday_dn.tga"  );
	load_cube_map_side( cube_map_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "textures/arrakisday_lf.tga" ) ;
	load_cube_map_side( cube_map_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, "textures/arrakisday_rt.tga" ) ;

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	//Otros

    mat4 aux;
	bool avanza=true;

    while (!glfwWindowShouldClose(g_window)){

        // Variables Previas a Ejecución

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
		if (deltaTime < 0.0166) continue;
        lastFrame = currentFrame;
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		glUseProgram (skybox_shader);
		glDepthMask( GL_FALSE );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_CUBE_MAP, cube_map_texture );
		glBindVertexArray( vaosky );
		glUniformMatrix4fv (view_skybox, 1, GL_FALSE, &glm::mat4(glm::mat3(view))[0][0]);
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glDepthMask( GL_TRUE );
	
		glUseProgram (shader_programme);
        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
		mundo->getDynamicsWorld()->stepSimulation(1/60.f, 10);

        btTransform trans;

		//Visualización de Objetos en la pantalla

		highway->reset_matrix();
		highway->draw(model_mat_location);
		box->reset_matrix();
		box->draw(model_mat_location);
		arena->reset_matrix();
		arena->draw(model_mat_location);

		automovil->showObject(trans, aux, car, model_mat_location);

		piedra00->showObject(trans, aux, rock, model_mat_location);
		piedra01->showObject(trans, aux, rock, model_mat_location);
		piedra02->showObject(trans, aux, rock, model_mat_location);
		piedra03->showObject(trans, aux, rock, model_mat_location);
		piedra04->showObject(trans, aux, rock, model_mat_location);
		piedra05->showObject(trans, aux, rock, model_mat_location);
		piedra06->showObject(trans, aux, rock, model_mat_location);
		piedra07->showObject(trans, aux, rock, model_mat_location);

		arena00->showObject(trans, aux, arena, model_mat_location);
		arena01->showObject(trans, aux, arena, model_mat_location);
		arena02->showObject(trans, aux, arena, model_mat_location);
		arena03->showObject(trans, aux, arena, model_mat_location);
		arena04->showObject(trans, aux, arena, model_mat_location);
		arena05->showObject(trans, aux, arena, model_mat_location);
		arena06->showObject(trans, aux, arena, model_mat_location);
		arena07->showObject(trans, aux, arena, model_mat_location);

		carretera0->showObject(trans, aux, highway, model_mat_location);
		carretera1->showObject(trans, aux, highway, model_mat_location);
		carretera2->showObject(trans, aux, highway, model_mat_location);
		carretera3->showObject(trans, aux, highway, model_mat_location);

		//caja01->showObject(trans, aux, box, model_mat_location);
		caja02->showObject(trans, aux, box, model_mat_location);
		caja03->showObject(trans, aux, box, model_mat_location);
		caja04->showObject(trans, aux, box, model_mat_location);
		caja05->showObject(trans, aux, box, model_mat_location);
		caja06->showObject(trans, aux, box, model_mat_location);
		caja07->showObject(trans, aux, box, model_mat_location);
		caja08->showObject(trans, aux, box, model_mat_location);
		caja09->showObject(trans, aux, box, model_mat_location);
		caja10->showObject(trans, aux, box, model_mat_location);
		caja11->showObject(trans, aux, box, model_mat_location);
		caja12->showObject(trans, aux, box, model_mat_location);
		caja13->showObject(trans, aux, box, model_mat_location);
		caja14->showObject(trans, aux, box, model_mat_location);
		caja15->showObject(trans, aux, box, model_mat_location);
		caja16->showObject(trans, aux, box, model_mat_location);
		caja17->showObject(trans, aux, box, model_mat_location);
		caja18->showObject(trans, aux, box, model_mat_location);
		caja19->showObject(trans, aux, box, model_mat_location);
		caja20->showObject(trans, aux, box, model_mat_location);
		caja21->showObject(trans, aux, box, model_mat_location);
		caja22->showObject(trans, aux, box, model_mat_location);
		caja23->showObject(trans, aux, box, model_mat_location);
		caja24->showObject(trans, aux, box, model_mat_location);
		caja25->showObject(trans, aux, box, model_mat_location);
		caja26->showObject(trans, aux, box, model_mat_location);
		caja27->showObject(trans, aux, box, model_mat_location);
		caja28->showObject(trans, aux, box, model_mat_location);
		caja29->showObject(trans, aux, box, model_mat_location);
		caja30->showObject(trans, aux, box, model_mat_location);
		caja31->showObject(trans, aux, box, model_mat_location);
		caja32->showObject(trans, aux, box, model_mat_location);
		caja33->showObject(trans, aux, box, model_mat_location);


		avanza=processInput(g_window, automovil, avanza);

		//Otros

        glfwSwapBuffers(g_window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;

}

//Métodos

bool processInput(GLFWwindow *window, Car *automovil, bool avanza){
	float cameraSpeed = 30. * deltaTime;
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

	if (avanza==false){
		cameraPos += cameraSpeed * cameraFront;
		automovil->setVelocity(btVector3(0,0,-30));
		lugar=lugar+30;
	}
	////////

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		if(avanza==true){
			avanza=false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && avanza==false){
		for (int i=1 ; i<30 ; i++){
			automovil->setVelocity(btVector3(0,0,i-30));
		}
		avanza=true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && avanza==false){
		automovil->setVelocity(btVector3(-15,0,-30));
		lugar=lugar+30;
       	//cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && avanza==false){
		automovil->setVelocity(btVector3(15,0,-30));
		lugar=lugar+30;
       	//cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		//cameraPos += cameraSpeed * cameraFront;
	}
	if(lugar>=57570){
		glfwSetWindowShouldClose(window, true);
	}
	return avanza;
}
