#ifndef CAMERA_H
#define CAMERA_H
#include "math3d.h"
struct Camera {
    Vec3 pos={0,15,0};
    float yaw=0, pitch=0;
    Vec3 front={0,0,-1}, up={0,1,0}, right={1,0,0};
    void update();
    Mat4 getView();
    void moveForward(float d), moveBackward(float d), moveLeft(float d), moveRight(float d);
};
#endif
