#include "shader.h"
#include <iostream>
static GLuint compile(GLenum type, const char* src) {
    GLuint s=glCreateShader(type);
    glShaderSource(s,1,&src,NULL);
    glCompileShader(s);
    int ok; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){ char log[512]; glGetShaderInfoLog(s,512,NULL,log); std::cerr<<"Shader Error:"<<log<<std::endl; }
    return s;
}
Shader::Shader(const char* vs, const char* fs) {
    GLuint v=compile(GL_VERTEX_SHADER,vs);
    GLuint f=compile(GL_FRAGMENT_SHADER,fs);
    prog=glCreateProgram();
    glAttachShader(prog,v); glAttachShader(prog,f);
    glLinkProgram(prog);
    glDeleteShader(v); glDeleteShader(f);
}
Shader::~Shader(){ glDeleteProgram(prog); }
void Shader::use(){ glUseProgram(prog); }
void Shader::setInt(const char* n, int v){ glUniform1i(glGetUniformLocation(prog,n),v); }
void Shader::setMat4(const char* n, float* m){ glUniformMatrix4fv(glGetUniformLocation(prog,n),1,GL_FALSE,m); }
