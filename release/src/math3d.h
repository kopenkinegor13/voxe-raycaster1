#ifndef MATH3D_H
#define MATH3D_H
#include <cmath>
struct Vec3 { float x,y,z; };
struct Mat4 { float m[16]; };
static Mat4 mat4_identity() { Mat4 r={0}; r.m[0]=r.m[5]=r.m[10]=r.m[15]=1; return r; }
static Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r={0};
    for(int c=0;c<4;c++) for(int row=0;row<4;row++)
        for(int k=0;k<4;k++) r.m[c*4+row] += a.m[c*4+k] * b.m[k*4+row];
    return r;
}
static Mat4 mat4_perspective(float fov, float asp, float n, float f) {
    Mat4 r={0}; float tanH=tan(fov*0.5f);
    r.m[0]=1/(asp*tanH); r.m[5]=1/tanH;
    r.m[10]=(f+n)/(n-f); r.m[11]=-1;
    r.m[14]=(2*f*n)/(n-f); r.m[15]=0;
    return r;
}
static Mat4 mat4_lookat(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f={center.x-eye.x, center.y-eye.y, center.z-eye.z};
    float l=sqrt(f.x*f.x+f.y*f.y+f.z*f.z); f.x/=l; f.y/=l; f.z/=l;
    Vec3 s={f.y*up.z-f.z*up.y, f.z*up.x-f.x*up.z, f.x*up.y-f.y*up.x};
    l=sqrt(s.x*s.x+s.y*s.y+s.z*s.z); s.x/=l; s.y/=l; s.z/=l;
    Vec3 u={s.y*f.z-s.z*f.y, s.z*f.x-s.x*f.z, s.x*f.y-s.y*f.x};
    Mat4 r={0};
    r.m[0]=s.x; r.m[4]=s.y; r.m[8]=s.z;
    r.m[1]=u.x; r.m[5]=u.y; r.m[9]=u.z;
    r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
    r.m[12]=-(s.x*eye.x+s.y*eye.y+s.z*eye.z);
    r.m[13]=-(u.x*eye.x+u.y*eye.y+u.z*eye.z);
    r.m[14]=f.x*eye.x+f.y*eye.y+f.z*eye.z;
    r.m[15]=1;
    return r;
}
#endif
