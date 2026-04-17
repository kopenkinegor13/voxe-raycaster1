#include "chunk.h"
#include <cmath>
#include <random>
#include <algorithm>
static int perm[512]; static bool noiseReady=false;
void initNoise(int seed){
    if(noiseReady) return;
    for(int i=0;i<256;i++) perm[i]=i;
    std::mt19937 g(seed);
    std::shuffle(perm, perm+256, g);
    for(int i=0;i<256;i++) perm[i+256]=perm[i];
    noiseReady=true;
}
static double fade(double t){ return t*t*t*(t*(t*6-15)+10); }
static double lerp(double a, double b, double t){ return a+t*(b-a); }
static double grad(int hash, double x, double y){
    int h=hash&3; double u=h<2?x:y, v=h<2?y:x;
    return ((h&1)?-u:u) + ((h&2)?-2.0*v:2.0*v);
}
static double noise2D(double x, double y){
    int X=(int)floor(x)&255, Y=(int)floor(y)&255;
    x-=floor(x); y-=floor(y);
    double u=fade(x), v=fade(y);
    int A=perm[X]+Y, B=perm[X+1]+Y;
    return lerp(lerp(grad(perm[A],x,y), grad(perm[B],x-1,y), u),
                lerp(grad(perm[A+1],x,y-1), grad(perm[B+1],x-1,y-1), u), v);
}
static double octaveNoise(double x, double y, int oct, double pers){
    double t=0,f=1,a=1,m=0;
    for(int i=0;i<oct;i++){ t+=noise2D(x*f,y*f)*a; m+=a; a*=pers; f*=2; }
    return t/m;
}
void Chunk::generate(){
    initNoise(42);
    for(int x=0;x<CS;x++) for(int z=0;z<CS;z++){
        double wx=cx*CS+x, wz=cz*CS+z;
        double h=octaveNoise(wx*0.015, wz*0.015, 5, 0.45);
        int height=(int)(14 + h*12); if(height<2) height=2;
        for(int y=0;y<CH;y++){
            if(y==0) data[x][y][z]=BEDROCK;
            else if(y<height-4) data[x][y][z]=STONE;
            else if(y<height) data[x][y][z]=DIRT;
            else if(y==height) data[x][y][z]=GRASS;
            else data[x][y][z]=AIR;
        }
        if(height<11){ for(int y=height+1; y<=11; y++) data[x][y][z]=WATER; if(height==10) data[x][11][z]=SAND; }
        if(height>11 && height<24 && octaveNoise(wx*0.08, wz*0.08, 2, 0.5)>0.6 && (x*7 + z*13)%5==0){
            for(int i=0;i<4;i++) if(height+1+i<CH) data[x][height+1+i][z]=WOOD;
            for(int dx=-2;dx<=2;dx++) for(int dz=-2;dz<=2;dz++) for(int dy=3;dy<=4;dy++)
                if(abs(dx)+abs(dz)<3 && height+dy<CH) data[x+dx][height+dy][z+dz]=LEAVES;
        }
    }
}
bool Chunk::isSolid(int x,int y,int z){
    if(x<0||x>=CS||y<0||y>=CH||z<0||z>=CS) return false;
    Block b=data[x][y][z]; return b!=AIR && b!=WATER;
}
Block Chunk::getBlock(int x,int y,int z){
    if(x<0||x>=CS||y<0||y>=CH||z<0||z>=CS) return AIR;
    return data[x][y][z];
}
void Chunk::buildMesh(){
    verts.clear();
    float cols[9][3]={
        {0,0,0}, {0.2,0.4,0.9}, {0.85,0.8,0.5}, {0.55,0.35,0.2},
        {0.3,0.7,0.3}, {0.5,0.5,0.5}, {0.2,0.2,0.2}, {0.4,0.25,0.1}, {0.1,0.4,0.1}
    };
    struct F{int nx,ny,nz; float v[18];};
    F faces[]={
        {1,0,0,  {1,0,0, 1,1,0, 1,1,1, 1,0,0, 1,1,1, 1,0,1}},
        {-1,0,0, {0,0,1, 0,1,1, 0,1,0, 0,0,1, 0,1,0, 0,0,0}},
        {0,1,0,  {0,1,1, 1,1,1, 1,1,0, 0,1,1, 1,1,0, 0,1,0}},
        {0,-1,0, {0,0,0, 1,0,0, 1,0,1, 0,0,0, 1,0,1, 0,0,1}},
        {0,0,1,  {0,0,1, 1,0,1, 1,1,1, 0,0,1, 1,1,1, 0,1,1}},
        {0,0,-1, {1,0,0, 0,0,0, 0,1,0, 1,0,0, 0,1,0, 1,1,0}}
    };
    float wx = cx * CS; float wz = cz * CS;
    for(int x=0;x<CS;x++) for(int y=0;y<CH;y++) for(int z=0;z<CS;z++){
        Block b=data[x][y][z];
        if(b==AIR) continue;
        for(auto& f:faces){
            if(!isSolid(x+f.nx,y+f.ny,z+f.nz)){
                float* c=cols[b];
                for(int i=0;i<18;i+=3){
                    verts.push_back(wx + x + f.v[i]);   verts.push_back(y + f.v[i+1]); verts.push_back(wz + z + f.v[i+2]);
                    verts.push_back(c[0]);       verts.push_back(c[1]);       verts.push_back(c[2]);
                }
            }
        }
    }
}
