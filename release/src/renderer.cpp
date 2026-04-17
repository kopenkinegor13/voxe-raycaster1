#include "renderer.h"
void Renderer::upload(int id, const std::vector<float>& v){
    if(id>=100) return;
    Mesh& m=meshes[id];
    if(m.vao) { glDeleteVertexArrays(1,&m.vao); glDeleteBuffers(1,&m.vbo); }
    glGenVertexArrays(1,&m.vao); glGenBuffers(1,&m.vbo);
    glBindVertexArray(m.vao); glBindBuffer(GL_ARRAY_BUFFER,m.vbo);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(float),v.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    m.count=v.size()/6; count++;
}
void Renderer::drawAll(){
    for(int i=0;i<count;i++) { glBindVertexArray(meshes[i].vao); glDrawArrays(GL_TRIANGLES,0,meshes[i].count); }
}
void Renderer::cleanup(){
    for(int i=0;i<count;i++){ glDeleteVertexArrays(1,&meshes[i].vao); glDeleteBuffers(1,&meshes[i].vbo); }
    count=0;
}
