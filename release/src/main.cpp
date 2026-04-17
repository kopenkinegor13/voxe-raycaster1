#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include "math3d.h"
#include "shader.h"
#include "camera.h"
#include "chunk.h"
#include "renderer.h"

static const char* vsSrc =
"#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"layout(location=1) in vec3 aColor;\n"
"out vec3 vColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"void main(){ vColor=aColor; gl_Position=proj*view*model*vec4(aPos,1.0); }";

static const char* fsSrc =
"#version 330 core\n"
"in vec3 vColor;\n"
"out vec4 FragColor;\n"
"uniform int isWireframe;\n"
"void main(){\n"
"   vec3 finalColor = vColor;\n"
"   if(isWireframe == 1) finalColor = vec3(0.1, 0.1, 0.1);\n"
"   FragColor = vec4(finalColor, 1.0);\n"
"}";

const int MAP = 6;
Chunk chunks[MAP][MAP];
Block getBlockWorld(int wx, int wy, int wz) {
    if(wy < 0 || wy >= CH) return AIR;
    int cx = wx / CS, cz = wz / CS;
    if(cx < 0 || cx >= MAP || cz < 0 || cz >= MAP) return AIR;
    return chunks[cx][cz].getBlock(wx % CS, wy, wz % CS);
}
struct Player { Vec3 pos, vel; bool onGround; float w=0.3f, h=1.7f; } player;
void physicsStep(float dt) {
    if(!player.onGround) player.vel.y -= 28.0f * dt;
    player.vel.y = std::max(player.vel.y, -35.0f);
    player.pos.x += player.vel.x * dt;
    int bx = (int)(player.pos.x + (player.vel.x > 0 ? player.w : -player.w));
    if(getBlockWorld(bx, (int)player.pos.y, (int)player.pos.z)!=AIR || getBlockWorld(bx, (int)(player.pos.y+player.h), (int)player.pos.z)!=AIR) {
        player.pos.x -= player.vel.x * dt; player.vel.x = 0;
    }
    player.pos.z += player.vel.z * dt;
    int bz = (int)(player.pos.z + (player.vel.z > 0 ? player.w : -player.w));
    if(getBlockWorld((int)player.pos.x, (int)player.pos.y, bz)!=AIR || getBlockWorld((int)player.pos.x, (int)(player.pos.y+player.h), bz)!=AIR) {
        player.pos.z -= player.vel.z * dt; player.vel.z = 0;
    }
    player.pos.y += player.vel.y * dt;
    player.onGround = false;
    if(player.vel.y < 0) {
        int by = (int)player.pos.y;
        if(getBlockWorld((int)player.pos.x, by, (int)player.pos.z) != AIR) {
            player.pos.y = by + 1.0f; player.vel.y = 0; player.onGround = true;
        }
    } else {
        int by = (int)(player.pos.y + player.h);
        if(getBlockWorld((int)player.pos.x, by, (int)player.pos.z) != AIR) {
            player.pos.y = by - player.h; player.vel.y = 0;
        }
    }
    float fric = player.onGround ? 0.85f : 0.96f;
    player.vel.x *= fric; player.vel.z *= fric;
}

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* win=SDL_CreateWindow("Voxel Engine",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,600,SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    SDL_GLContext ctx=SDL_GL_CreateContext(win);
    glewExperimental=GL_TRUE; glewInit();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    Shader shader(vsSrc,fsSrc);
    for(int x=0;x<MAP;x++) for(int z=0;z<MAP;z++){ chunks[x][z].cx=x; chunks[x][z].cz=z; chunks[x][z].generate(); chunks[x][z].buildMesh(); }
    Renderer rend;
    for(int x=0;x<MAP;x++) for(int z=0;z<MAP;z++) rend.upload(x*MAP+z, chunks[x][z].verts);

    player.pos={48,28,48}; Camera cam; cam.update();
    bool quit=false; SDL_Event e; SDL_SetRelativeMouseMode(SDL_TRUE);
    float sens=0.2f; Uint32 last=SDL_GetTicks();

    while(!quit){
        Uint32 now=SDL_GetTicks(); float dt=(now-last)/1000.0f; last=now; if(dt>0.1f) dt=0.1f;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT||(e.type==SDL_KEYDOWN&&e.key.keysym.sym==SDLK_ESCAPE)) quit=true;
            if(e.type==SDL_MOUSEMOTION){
                cam.yaw+=e.motion.xrel*sens; cam.pitch-=e.motion.yrel*sens;
                cam.pitch=std::max(-89.0f,std::min(89.0f,cam.pitch)); cam.update();
            }
        }
        const Uint8* k=SDL_GetKeyboardState(NULL);
        Vec3 fwd=cam.front; fwd.y=0; float l=sqrt(fwd.x*fwd.x+fwd.z*fwd.z); fwd.x/=l; fwd.z/=l;
        Vec3 rgt={fwd.z,0,-fwd.x};
        float spd=9.0f;
        Vec3 inp={0,0,0};
        if(k[SDL_SCANCODE_W]) { inp.x+=fwd.x; inp.z+=fwd.z; }
        if(k[SDL_SCANCODE_S]) { inp.x-=fwd.x; inp.z-=fwd.z; }
        if(k[SDL_SCANCODE_A]) { inp.x-=rgt.x; inp.z-=rgt.z; }
        if(k[SDL_SCANCODE_D]) { inp.x+=rgt.x; inp.z+=rgt.z; }
        float il=sqrt(inp.x*inp.x+inp.z*inp.z);
        if(il>0.01f){ inp.x/=il; inp.z/=il; player.vel.x=inp.x*spd; player.vel.z=inp.z*spd; }
        else { player.vel.x*=0.8f; player.vel.z*=0.8f; }
        if(k[SDL_SCANCODE_SPACE]&&player.onGround) player.vel.y=11.0f;

        physicsStep(dt);
        cam.pos=player.pos; cam.pos.y+=1.6f; cam.update();

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(0.53,0.81,0.92,1);
        shader.use();
        shader.setMat4("proj", mat4_perspective(70.0f*0.01745f,800.0/600,0.1,200.0).m);
        shader.setMat4("view", cam.getView().m);
        shader.setMat4("model", mat4_identity().m);

        // Pass 1: Solid Color Blocks
        shader.setInt("isWireframe", 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        rend.drawAll();

        // Pass 2: Black Wireframe Contour
        shader.setInt("isWireframe", 1);
        glLineWidth(1.2f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        rend.drawAll();
        
        // Reset
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        SDL_GL_SwapWindow(win); SDL_Delay(1);
    }
    SDL_GL_DeleteContext(ctx); SDL_DestroyWindow(win); SDL_Quit(); return 0;
}
