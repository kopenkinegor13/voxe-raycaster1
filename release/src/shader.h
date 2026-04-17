#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>
#include <string>
class Shader {
public:
    Shader(const char* vs, const char* fs);
    ~Shader();
    void use();
    void setInt(const char* n, int v);
    void setMat4(const char* n, float* m);
    GLuint getID() const { return prog; }
private:
    GLuint prog;
};
#endif
