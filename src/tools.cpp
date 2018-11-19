#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "gl_utils.h"
#include "tools.hpp"

#define PI 3.14159265359

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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

bool load_mesh (const char* file_name, GLuint* vao, int* point_count) {
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate);
	if (!scene) {
		fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}
	printf ("  %i animations\n", scene->mNumAnimations);
	printf ("  %i cameras\n", scene->mNumCameras);
	printf ("  %i lights\n", scene->mNumLights);
	printf ("  %i materials\n", scene->mNumMaterials);
	printf ("  %i meshes\n", scene->mNumMeshes);
	printf ("  %i textures\n", scene->mNumTextures);
	const aiMesh* mesh = scene->mMeshes[0];
	printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
	*point_count = mesh->mNumVertices;
	glGenVertexArrays (1, vao);
	glBindVertexArray (*vao);
	GLfloat* points = NULL;
	GLfloat* normals = NULL;
	GLfloat* texcoords = NULL;
	if (mesh->HasPositions ()) {
		points = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
			points[i * 3] = (GLfloat)vp->x;
			points[i * 3 + 1] = (GLfloat)vp->y;
			points[i * 3 + 2] = (GLfloat)vp->z;
		}
	}
	if (mesh->HasNormals ()) {
		normals = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vn = &(mesh->mNormals[i]);
			normals[i * 3] = (GLfloat)vn->x;
			normals[i * 3 + 1] = (GLfloat)vn->y;
			normals[i * 3 + 2] = (GLfloat)vn->z;
		}
	}
	if (mesh->HasTextureCoords (0)) {
		texcoords = (GLfloat*)malloc (*point_count * 2 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
			texcoords[i * 2] = (GLfloat)vt->x;
			texcoords[i * 2 + 1] = (GLfloat)vt->y;
		}
	}

	/* copy mesh data into VBOs */
	if (mesh->HasPositions ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			points,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);
		free (points);
	}
	if (mesh->HasNormals ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			normals,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);
		free (normals);
	}
	if (mesh->HasTextureCoords (0)) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			2 * *point_count * sizeof (GLfloat),
			texcoords,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (2);
		free (texcoords);
	}
	if (mesh->HasTangentsAndBitangents ()) {
	}
	aiReleaseImport (scene);
	printf ("mesh loaded\n");
	return true;
}

btCollisionShape* load_mesh_point_static (const char* file_name) {
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate);
	btCollisionShape *_shape = NULL;
	if (!scene) {
		fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		return NULL;
	}
	const aiMesh* mesh = scene->mMeshes[0];
	printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
	int point_count = mesh->mNumVertices;
	btTriangleMesh* meshTriangle = new btTriangleMesh();
	if (mesh->HasPositions ()) {
		for (int i = 0; i < point_count; i+=3) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
			const aiVector3D* vp1 = &(mesh->mVertices[i+1]);
			const aiVector3D* vp2 = &(mesh->mVertices[i+2]);
            btVector3 bv1 = btVector3(vp->x, vp->y, vp->z);
            btVector3 bv2 = btVector3(vp1->x, vp1->y, vp1->z);
            btVector3 bv3 = btVector3(vp2->x, vp2->y, vp2->z);
            meshTriangle->addTriangle(bv1, bv2, bv3);
		}
		_shape = new btBvhTriangleMeshShape(meshTriangle, true);
	}
	aiReleaseImport (scene);
	return _shape;
}

btCollisionShape* load_mesh_point (const char* file_name) {
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate);
	btCollisionShape *_shape = NULL;
	if (!scene) {
		fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		return NULL;
	}
	const aiMesh* mesh = scene->mMeshes[0];
	printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
	int point_count = mesh->mNumVertices;
	_shape = new btConvexHullShape();
	if (mesh->HasPositions ()) {
		for (int i = 0; i < point_count; i++) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
            btVector3 bv1 = btVector3(vp->x, vp->y, vp->z);
            ((btConvexHullShape*)_shape)->addPoint(bv1, true);
		}
	}
	aiReleaseImport (scene);
	btShapeHull* hull = new btShapeHull((btConvexShape*)_shape);
	btScalar margin = _shape->getMargin();
	hull->buildHull(margin);
	btConvexHullShape* simplifedConvexShape = new btConvexHullShape((const btScalar*)hull->getVertexPointer(), hull->numVertices(), sizeof(btVector3));
	return simplifedConvexShape;
}