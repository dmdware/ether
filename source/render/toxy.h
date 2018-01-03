

#ifndef TOXY_H
#define TOXY_H

#include "../math/v3f.h"
#include "../math/v4f.h"


float vol3f2(v3f a, v3f b, v3f c);
float vol3f(v3f a, v3f b, v3f c, v3f d);
float sa3f(v3f a, v3f b, v3f c);
float vol4f2(float s1, float s2, float s3, float s4, float a1, float a2, float a3, float a4, float a5, float a6);
void v4fsub(v4f *v, v4f v1, v4f v2);
float vol4f(v4f a, v4f b, v4f c, v4f d);
float sa4f(v4f a, v4f b, v4f c);
v3f toxy2(v3f vi, float wx, float wy, v3f p[8]);
v3f toxy(v3f vi, float wx, float wy, v3f view, v3f pos, v3f up, v3f strafe, float maxd, float mind, float fov);


#endif