#ifndef TOOLS_H
#define TOOLS_H

#include <bullet/btBulletDynamicsCommon.h>
#include "gl_utils.h"

// funcion que carga una malla desde filename
bool load_mesh (const char* file_name, GLuint* vao, int* point_count);
// funcion gameplay
bool gameplay(float cam_speed, double elapsed_seconds, float *cam_pos, float *cam_yaw, float cam_yaw_speed);

btCollisionShape* load_mesh_point (const char* file_name);
#endif
