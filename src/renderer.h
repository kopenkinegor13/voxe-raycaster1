#ifndef RENDERER_H
#define RENDERER_H
#include <GL/glew.h>
#include <vector>
struct Mesh { GLuint vao, vbo; int count; };
class Renderer {
    Mesh meshes[100]; int count=0;
public:
    void upload(int id, const std::vector<float>& v);
    void drawAll();
    void cleanup();
};
#endif
