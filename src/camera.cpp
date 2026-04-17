#include "camera.h"
void Camera::update(){
    float yR = yaw * 0.0174532925f;
    float pR = pitch * 0.0174532925f;
    front.x = cos(pR) * cos(yR);
    front.y = sin(pR);
    front.z = cos(pR) * sin(yR);
    float l = sqrt(front.x*front.x + front.y*front.y + front.z*front.z);
    front.x/=l; front.y/=l; front.z/=l;
    right.x = front.y*up.z - front.z*up.y;
    right.y = front.z*up.x - front.x*up.z;
    right.z = front.x*up.y - front.y*up.x;
    l = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
    right.x/=l; right.y/=l; right.z/=l;
}
Mat4 Camera::getView(){ return mat4_lookat(pos, {pos.x+front.x, pos.y+front.y, pos.z+front.z}, up); }
void Camera::moveForward(float d){ pos.x+=front.x*d; pos.y+=front.y*d; pos.z+=front.z*d; }
void Camera::moveBackward(float d){ pos.x-=front.x*d; pos.y-=front.y*d; pos.z-=front.z*d; }
void Camera::moveLeft(float d){ pos.x-=right.x*d; pos.y-=right.y*d; pos.z-=right.z*d; }
void Camera::moveRight(float d){ pos.x+=right.x*d; pos.y+=right.y*d; pos.z+=right.z*d; }
