

#ifndef MESH_H
#define MESH_H

#include "../math/v3f.h"

struct mesh
{
	int nv;
	v3f *v;
};

typedef struct mesh mesh;

void makeq(mesh* m);

#endif