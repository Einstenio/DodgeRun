#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include "gl_utils.h"

bool load_mesh (const char* file_name, GLuint* vao, int* point_count);
bool gameplay(float cam_speed, double elapsed_seconds, float *cam_pos, float *cam_yaw, float cam_yaw_speed);
btCollisionShape* load_mesh_point (const char* file_name);
btCollisionShape* load_mesh_point_static (const char* file_name);
void init();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
#endif
