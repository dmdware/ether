

#include "mesh.h"

void makeq(mesh* m)
{
	int x, y, z;
	v3f a[4];

#define SX		0.7f
#define SNX		9

	m->nv = SNX*SNX * 6 * 2 * 3;
	m->v = (v3f*)malloc(m->nv * sizeof(v3f));

	for (x = 0; x < SNX; ++x)
	{
		for (y = 0; y < SNX; ++y)
		{
			a[0].x = (x - SNX / 2) / (float)SNX*SX;
			a[0].y = (y - SNX / 2) / (float)SNX*SX;
			a[0].z = (0 - SNX / 2) / (float)SNX*SX;

			a[1].x = (x - SNX / 2) / (float)SNX*SX;
			a[1].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[1].z = (0 - SNX / 2) / (float)SNX*SX;

			a[2].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[2].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[2].z = (0 - SNX / 2) / (float)SNX*SX;

			a[3].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[3].y = (y - SNX / 2) / (float)SNX*SX;
			a[3].z = (0 - SNX / 2) / (float)SNX*SX;

			m->v[(SNX*SNX * 0 + x + y*SNX) * 2 * 3 + 0 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 0 + x + y*SNX) * 2 * 3 + 0 * 3 + 1] = a[1];
			m->v[(SNX*SNX * 0 + x + y*SNX) * 2 * 3 + 0 * 3 + 2] = a[2];

			m->v[(SNX*SNX * 0 + x + y*SNX) * 2 * 3 + 1 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 0 + x + y*SNX) * 2 * 3 + 1 * 3 + 1] = a[2];
			m->v[(SNX*SNX * 0 + x + y*SNX) * 2 * 3 + 1 * 3 + 2] = a[3];
		}
	}

	for (x = 0; x < SNX; ++x)
	{
		for (y = 0; y < SNX; ++y)
		{
			a[0].x = (x - SNX / 2) / (float)SNX*SX;
			a[0].y = (y - SNX / 2) / (float)SNX*SX;
			a[0].z = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;

			a[1].x = (x - SNX / 2) / (float)SNX*SX;
			a[1].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[1].z = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;

			a[2].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[2].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[2].z = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;

			a[3].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[3].y = (y - SNX / 2) / (float)SNX*SX;
			a[3].z = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;

			m->v[(SNX*SNX * 1 + x + y*SNX) * 2 * 3 + 0 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 1 + x + y*SNX) * 2 * 3 + 0 * 3 + 1] = a[1];
			m->v[(SNX*SNX * 1 + x + y*SNX) * 2 * 3 + 0 * 3 + 2] = a[2];

			m->v[(SNX*SNX * 1 + x + y*SNX) * 2 * 3 + 1 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 1 + x + y*SNX) * 2 * 3 + 1 * 3 + 1] = a[2];
			m->v[(SNX*SNX * 1 + x + y*SNX) * 2 * 3 + 1 * 3 + 2] = a[3];
		}
	}


	for (y = 0; y < SNX; ++y)
	{
		for (z = 0; z < SNX; ++z) 
		{
			a[0].x = (0 - SNX / 2) / (float)SNX*SX;
			a[0].y = (y - SNX / 2) / (float)SNX*SX;
			a[0].z = (z - SNX / 2) / (float)SNX*SX;

			a[1].x = (0 - SNX / 2) / (float)SNX*SX;
			a[1].y = (y - SNX / 2) / (float)SNX*SX;
			a[1].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[2].x = (0 - SNX / 2) / (float)SNX*SX;
			a[2].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[2].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[3].x = (0 - SNX / 2) / (float)SNX*SX;
			a[3].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[3].z = (z - SNX / 2) / (float)SNX*SX;

			m->v[(SNX*SNX * 2 + y + z*SNX) * 2 * 3 + 0 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 2 + y + z*SNX) * 2 * 3 + 0 * 3 + 1] = a[1];
			m->v[(SNX*SNX * 2 + y + z*SNX) * 2 * 3 + 0 * 3 + 2] = a[2];

			m->v[(SNX*SNX * 2 + y + z*SNX) * 2 * 3 + 1 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 2 + y + z*SNX) * 2 * 3 + 1 * 3 + 1] = a[2];
			m->v[(SNX*SNX * 2 + y + z*SNX) * 2 * 3 + 1 * 3 + 2] = a[3];
		}
	}

	for (y = 0; y < SNX; ++y)
	{
		for (z = 0; z < SNX; ++z)
		{
			a[0].x = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[0].y = (y - SNX / 2) / (float)SNX*SX;
			a[0].z = (z - SNX / 2) / (float)SNX*SX;

			a[1].x = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[1].y = (y - SNX / 2) / (float)SNX*SX;
			a[1].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[2].x = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[2].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[2].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[3].x = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[3].y = (y + 1 - SNX / 2) / (float)SNX*SX;
			a[3].z = (z - SNX / 2) / (float)SNX*SX;

			m->v[(SNX*SNX * 3 + y + z*SNX) * 2 * 3 + 0 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 3 + y + z*SNX) * 2 * 3 + 0 * 3 + 1] = a[1];
			m->v[(SNX*SNX * 3 + y + z*SNX) * 2 * 3 + 0 * 3 + 2] = a[2];

			m->v[(SNX*SNX * 3 + y + z*SNX) * 2 * 3 + 1 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 3 + y + z*SNX) * 2 * 3 + 1 * 3 + 1] = a[2];
			m->v[(SNX*SNX * 3 + y + z*SNX) * 2 * 3 + 1 * 3 + 2] = a[3];
		}
	}


	for (x = 0; x < SNX; ++x)
	{
		for (z = 0; z < SNX; ++z)
		{
			a[0].x = (x - SNX / 2) / (float)SNX*SX;
			a[0].y = (0 - SNX / 2) / (float)SNX*SX;
			a[0].z = (z - SNX / 2) / (float)SNX*SX;

			a[1].x = (y - SNX / 2) / (float)SNX*SX;
			a[1].y = (0 - SNX / 2) / (float)SNX*SX;
			a[1].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[2].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[2].y = (0 - SNX / 2) / (float)SNX*SX;
			a[2].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[3].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[3].y = (0 - SNX / 2) / (float)SNX*SX;
			a[3].z = (z - SNX / 2) / (float)SNX*SX;

			m->v[(SNX*SNX * 4 + x + z*SNX) * 2 * 3 + 0 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 4 + x + z*SNX) * 2 * 3 + 0 * 3 + 1] = a[1];
			m->v[(SNX*SNX * 4 + x + z*SNX) * 2 * 3 + 0 * 3 + 2] = a[2];

			m->v[(SNX*SNX * 4 + x + z*SNX) * 2 * 3 + 1 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 4 + x + z*SNX) * 2 * 3 + 1 * 3 + 1] = a[2];
			m->v[(SNX*SNX * 4 + x + z*SNX) * 2 * 3 + 1 * 3 + 2] = a[3];
		}
	}

	for (x = 0; x < SNX; ++x)
	{
		for (z = 0; z < SNX; ++z)
		{
			a[0].x = (x - SNX / 2) / (float)SNX*SX;
			a[0].y = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[0].z = (z - SNX / 2) / (float)SNX*SX;

			a[1].x = (x - SNX / 2) / (float)SNX*SX;
			a[1].y = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[1].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[2].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[2].y = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[2].z = (z + 1 - SNX / 2) / (float)SNX*SX;

			a[3].x = (x + 1 - SNX / 2) / (float)SNX*SX;
			a[3].y = (SNX - 1 - SNX / 2 + 1) / (float)SNX*SX;
			a[3].z = (z - SNX / 2) / (float)SNX*SX;

			m->v[(SNX*SNX * 5 + x + z*SNX) * 2 * 3 + 0 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 5 + x + z*SNX) * 2 * 3 + 0 * 3 + 1] = a[1];
			m->v[(SNX*SNX * 5 + x + z*SNX) * 2 * 3 + 0 * 3 + 2] = a[2];

			m->v[(SNX*SNX * 5 + x + z*SNX) * 2 * 3 + 1 * 3 + 0] = a[0];
			m->v[(SNX*SNX * 5 + x + z*SNX) * 2 * 3 + 1 * 3 + 1] = a[2];
			m->v[(SNX*SNX * 5 + x + z*SNX) * 2 * 3 + 1 * 3 + 2] = a[3];
		}
	}

#undef SNX
#undef SX

}