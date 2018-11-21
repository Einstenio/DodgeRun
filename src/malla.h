#ifndef MALLA_H
#define MALLA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
using namespace std;

class malla{
    private:
        GLuint vao, vbo;
        int numVertices;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::mat4 modelMatrix;
        char* filename;
        float angle;

    public:
        malla(char *filename);
        GLuint getVao();
        int getNumVertices();
        glm::vec3 getPosition();
        glm::vec3 getRotation();
        char* getFilename();
        void setVao(GLuint vao);
        void setNumVertices(int num);
        void setPosition(glm::vec3 pos);
        void setRotation(float ang, glm::vec3 rot);
        void setFilename(char *f);
        void setModelMatrix(glm::mat4 model);
        void draw(int matloc);
};

#endif
