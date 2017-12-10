


#include "appmain.h"
#include "../gui/layouts/appgui.h"
#include "../sys/utils.h"
#include "../math/v3f.h"
#include "../math/matf.h"
#include "../math/3dmath.h"
#include "../sys/syswin.h"
#include "../gui/wg.h"
#include "../gui/gui.h"
#include "../sim/simdef.h"
#include "../sim/user.h"
#include "../sound/sound.h"
#include "../net/net.h"
#include "../gui/font.h"
#include "../sim/simflow.h"
#include "../sim/simvars.h"
#include "../render/shader.h"

char g_appmode = APPMODE_LOGO;
char g_viewmode = VIEWMODE_FIRST;
char g_restage = 0;


#ifdef PLAT_WIN
HINSTANCE g_hinst = NULL;
#endif

void loadsysres()
{
	loadfonts();
	loadcursors();
}

void upload()
{
	wg *gui, *menu;

	gui = (wg*)&g_gui;

	switch(g_restage)
	{
	case 0:
/*		if(!loadqmod()) */ g_restage++;
		break;
	case 1:
		if(!loadqtex())
		{

	//		lfree(&g_modload);
			vfree(&g_texload);

//			g_lastLMd = -1;
			g_lastloadtex = -1;

			g_appmode = APPMODE_MENU;
			wghide(gui);
			menu = wgget(gui, "menu");
			wgshow(menu);
		}
		break;
	}
}

void upreload()
{
	wg *gui, *load;

	gui = (wg*)&g_gui;

	g_restage = 0;
	g_lastloadtex = -1;
//	g_lastLMd = -1;
	wgfree(gui);
//	FreeModels();
	freesps();
	freetexs();
	breakwin(APPTIT);
	makewin(APPTIT);

	/* Important - VBO only possible after window GL context made. */
	g_appmode = APPMODE_LOADING;

	loadsysres();
	queuesimres();
	makewg();

	wghide(gui);
	load = wgget(gui, "loading");
	wgshow(load);

	/*
	TODO
	Overhaul ui system
	Make scrollable everywhere
	Croppable pipeline 
	*/
}

void update()
{
//	if(g_sock)
//		UpdNet();

	switch(g_appmode)
	{
	case APPMODE_LOGO:
		uplogo();
		break;
//	case APPMODE_INTRO:
//		upintro();
//		break;
	case APPMODE_LOADING:
		upload();
		break;
	case APPMODE_RELOADING:
		upreload();
		break;
	case APPMODE_PLAY:
		upsim();
		break;
	case APPMODE_EDITOR:
//		uped();
		break;
	}
}

float vol3f2(v3f a, v3f b, v3f c)
{
	float f; 
	b = cross3f(b, c); 
	f = dot3f(a, b); 
	f = fabs(f); 
	f *= 1.0 / (2.0 * 3.0); 
	return f; 
}

float vol3f(v3f a, v3f b, v3f c, v3f d)
{
	v3fsub(&a, a, d);
	v3fsub(&b, b, d);
	v3fsub(&c, c, d);
	return vol3f2(a, b, c); 
}

float sa3f(v3f a, v3f b, v3f c)
{
	float s, a1, a2, a3;
	v3f ab, ac, bc;

	v3fsub(&ab, a, b);
	v3fsub(&ac, a, c);
	v3fsub(&bc, b, c);

	a1 = mag3f(ab);
	a2 = mag3f(ac);
	a3 = mag3f(bc);

	s = (a1 + a2 + a3) / 2.0f;

	return sqrtf(s * (s - a1) * (s - a2) * (s - a3));
}

v3f toxy(v3f vi, float wx, float wy, v3f p[8])
{
	float v[12];
	int i;
	v[0] = vol3f(vi, p[0], p[1], p[2]) / sa3f(p[1], p[2], p[0]);
	v[1] = vol3f(vi, p[0], p[2], p[3]) / sa3f(p[0], p[3], p[2]);
	v[2] = vol3f(vi, p[1], p[2], p[6]) / sa3f(p[1], p[2], p[6]);
	v[3] = vol3f(vi, p[1], p[6], p[5]) / sa3f(p[1], p[6], p[5]);
	v[4] = vol3f(vi, p[2], p[7], p[6]) / sa3f(p[2], p[6], p[7]);
	v[5] = vol3f(vi, p[3], p[7], p[2]) / sa3f(p[7], p[3], p[2]);
	v[6] = vol3f(vi, p[3], p[0], p[7]) / sa3f(p[7], p[3], p[0]);
	v[7] = vol3f(vi, p[0], p[7], p[4]) / sa3f(p[7], p[4], p[0]);
	v[8] = vol3f(vi, p[1], p[5], p[4]) / sa3f(p[1], p[5], p[4]);
	v[9] = vol3f(vi, p[0], p[1], p[4]) / sa3f(p[0], p[1], p[4]);
	v[10] = vol3f(vi, p[5], p[6], p[7]) / sa3f(p[5], p[6], p[7]);
	v[11] = vol3f(vi, p[4], p[5], p[7]) / sa3f(p[4], p[5], p[7]);
	for (i = 0; i < 12; i++)
		fprintf(g_applog, "[%f,%f,%f]v[%d]=%f\r\n", vi.x, vi.y, vi.z, i, v[i]);
	fflush(g_applog);
	vi.x = wx * (v[4] + v[5]) / (v[8] + v[9] + v[4] + v[5]);
	vi.y = wy * (v[2] + v[3]) / (v[6] + v[7] + v[2] + v[3]);
	vi.z = ((v[10] + v[11]) / (v[0] + v[1] + v[10] + v[11]));
	return vi;
}

void linexy(v3f f, v3f t, int wx, int wy, char* im, float r, float g, float b)
{
	int i;
	v3f v;
	float fd;
	int c;

	v3fsub(&v, f, t);
	fd = mag3f(v);

	for (i = 0; i <= fd; ++i)
	{
		v3fsub(&v, f, t);
		v3fmul(&v, v, i / fd);
		v3fadd(&v, v, f);

		if ((int)v.x < 0 || (int)v.x >= wx || (int)v.y < 0 || (int)v.y >= wy)
			continue;

		c = (1.0 - (v.z - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE)) * 255;

		if (c <= (int)im[3 * (wx*((int)v.y) + ((int)v.x))])
			continue;

		im[3 * (wx*((int)v.y) + ((int)v.x))] = c * r;
		im[3 * (wx*((int)v.y) + ((int)v.x)) + 1] = c * g;
		im[3 * (wx*((int)v.y) + ((int)v.x)) + 2] = c * b;
	}
}

#define NPX	11
#define NP	(NPX*NPX*NPX)
static v3f *p = NULL;
static v3f *pvv = NULL;
char sasd = 0;

void drawscene(float* proj, float* viewmat, float* modelmat, float* modelviewinv, 
			   float mvLightPos[3], float lightDir[3])
{
	mf mvp;
	glshader *s;
	v3f v[3];
	int i;
	v3f vv;
	v3f dv;
	float d;
	v3f cv;
	v3f cv2;
	int x, y, z;
	v3f pv[8];
	v3f vvvv[3][2][2];
	char *im;
	texdata pm;
	static int ff = 0;
	v3f xy;
	char fm[123];
	float ta;

#define WX		512
#define WY		512

	if (!p)
	{
		p = (v3f*)malloc(sizeof(v3f)*NP);
		pvv = (v3f*)malloc(sizeof(v3f)*NP);
	}

	ff++;
	im = (char*)malloc(sizeof(char)*WX*WY * 3);

	pm.sizex = WX;
	pm.sizey = WY;
	pm.channels = 3;
	pm.data = im;

#if 01
	if (!sasd)
	{
		sasd = 1;

		for (i = 0; i < NP; ++i)
		{
			p[i].x = i%NPX;
			p[i].y = (i / NPX) % NPX;
			p[i].z = (i / NPX / NPX);
		}

		memset(pvv, 0, sizeof(float)*NP * 3);
	}

	for (i = 0; i < WX*WY * 3; ++i)
		im[i] = 0;

	cv.x = NPX / 2;
	cv.y = NPX / 2;
	cv.z = NPX / 2;
	cv2.x = NPX / 2;
	cv2.y = NPX / 4;
	cv2.z = 0;

	mfset(&mvp, viewmat);
	mfpostmult(&mvp, (mf*)proj);
#endif
	//mfset(&mvp, proj);
	//mfpostmult(&mvp, (mf*)viewmat);

	//return;
	usesh(SH_COLOR3D);
	s = g_shader+g_cursh;

	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);

#if 01
	g_camf.pos.x = 0;
	g_camf.pos.y = 0;
	g_camf.pos.z = -1;
	g_camf.strafe.x = 1;
	g_camf.strafe.y = 0;
	g_camf.strafe.z = 0;
	g_camf.up.x = 0;
	g_camf.up.y = 1;
	g_camf.up.z = 0;
	g_camf.view.x = 0;
	g_camf.view.y = 0;
	g_camf.view.z = 0;

#define X	0
#define Y	1
#define Z	2
#define NEARP	0
#define FARP		1
#define POS		0
#define NEG		1

#define FOV		90

	vvvv[X][NEARP][POS] = g_camf.strafe;
	v3fsub(&vvvv[Z][NEARP][POS], g_camf.view, g_camf.pos);
	vvvv[Z][NEARP][POS] = norm3f(vvvv[Z][NEARP][POS]);
	vvvv[Y][NEARP][POS] = norm3f(cross3f(vvvv[X][NEARP][POS], vvvv[Z][NEARP][POS]));
	ta = tan(FOV / 180.0f * 3.14159f / 2.0f);
	v3fmul(&vvvv[Y][NEARP][POS], vvvv[Y][NEARP][POS], WX / WY * ta);
	v3fmul(&vvvv[X][NEARP][POS], vvvv[X][NEARP][POS], ta);

	v3fmul(&vvvv[X][FARP][POS], vvvv[X][NEARP][POS], MAX_DISTANCE - MIN_DISTANCE);
	v3fmul(&vvvv[Y][FARP][POS], vvvv[Y][NEARP][POS], MAX_DISTANCE - MIN_DISTANCE);
	v3fmul(&vvvv[Z][FARP][POS], vvvv[Z][NEARP][POS], MAX_DISTANCE - MIN_DISTANCE);

	v3fmul(&vvvv[X][NEARP][NEG], vvvv[X][NEARP][POS], -1);
	v3fmul(&vvvv[Y][NEARP][NEG], vvvv[Y][NEARP][POS], -1);
	v3fmul(&vvvv[Z][NEARP][NEG], vvvv[Z][NEARP][POS], -1);
	v3fmul(&vvvv[X][FARP][NEG], vvvv[X][FARP][POS], -1);
	v3fmul(&vvvv[Y][FARP][NEG], vvvv[Y][FARP][POS], -1);
	v3fmul(&vvvv[Z][FARP][NEG], vvvv[Z][FARP][POS], -1);

	v3fadd(&pv[0], vvvv[X][FARP][NEG], vvvv[Y][FARP][POS]);
	v3fadd(&pv[0], pv[0], vvvv[Z][FARP][POS]);
	v3fadd(&pv[1], vvvv[X][FARP][POS], vvvv[Y][FARP][POS]);
	v3fadd(&pv[1], pv[1], vvvv[Z][FARP][POS]);
	v3fadd(&pv[2], vvvv[X][FARP][POS], vvvv[Y][FARP][NEG]);
	v3fadd(&pv[2], pv[2], vvvv[Z][FARP][POS]);
	v3fadd(&pv[3], vvvv[X][FARP][NEG], vvvv[Y][FARP][NEG]);
	v3fadd(&pv[3], pv[3], vvvv[Z][FARP][POS]);
	v3fadd(&pv[4], vvvv[X][NEARP][NEG], vvvv[Y][NEARP][POS]);
	v3fadd(&pv[4], pv[4], vvvv[Z][NEARP][POS]);
	v3fadd(&pv[5], vvvv[X][NEARP][POS], vvvv[Y][NEARP][POS]);
	v3fadd(&pv[5], pv[5], vvvv[Z][NEARP][POS]);
	v3fadd(&pv[6], vvvv[X][NEARP][POS], vvvv[Y][NEARP][NEG]);
	v3fadd(&pv[6], pv[6], vvvv[Z][NEARP][POS]);
	v3fadd(&pv[7], vvvv[X][NEARP][NEG], vvvv[Y][NEARP][NEG]);
	v3fadd(&pv[7], pv[7], vvvv[Z][NEARP][POS]);

	pv[0].x -= 1000;
	pv[0].y += 50;
	pv[1].y += 100;
	pv[1].z += 100000;
	pv[2].z -= 100;

#undef X
#undef Y
#undef Z
#undef NEARP
#undef FARP
#undef POS
#undef NEG

	v3fadd(&pv[0], pv[0], g_camf.pos);
	v3fadd(&pv[1], pv[1], g_camf.pos);
	v3fadd(&pv[2], pv[2], g_camf.pos);
	v3fadd(&pv[3], pv[3], g_camf.pos);
	v3fadd(&pv[4], pv[4], g_camf.pos);
	v3fadd(&pv[5], pv[5], g_camf.pos);
	v3fadd(&pv[6], pv[6], g_camf.pos);
	v3fadd(&pv[7], pv[7], g_camf.pos);

	//glUniform3fv(s->slot[SSLOT_P], 8, pv);

#if 0
	v[0].x = 2.0f;
	v[0].y = 5.0f;
	v[0].z = -50.0f;
	v[1].x = 7.0f;
	v[1].y = 1.0f;
	v[1].z = -50.0f;
	v[2].x = 2.0f;
	v[2].y = 1.0f;
	v[2].z = -50.0f;
#endif

	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);

	for (x = 0; x < NPX; ++x)
	{
		for (y = 0; y < NPX; ++y)
		{
			for (z = 0; z < NPX; ++z)
			//z = 0;
			{
				v[0] = p[z*NPX*NPX + y*NPX + x];
				//xy = toxy(v[0], WX, WY, pv);
				//im[0] = 255;

				//xy.x += 1;

#if 0
				if (xy.x >= 0 && xy.x < WX && xy.y >= 0 && xy.y < WY)
				{
					//fprintf(g_applog, "xy.x%f %f\r\n", xy.x, xy.y);
					//fflush(g_applog);
					im[((int)xy.x%WX) * 3 + 3 * WX * ((int)xy.y%WY)] = 255;
					im[((int)xy.x%WX) * 3 + 3 * WX * ((int)xy.y%WY) + 1] = 255;
					im[((int)xy.x%WX) * 3 + 3 * WX * ((int)xy.y%WY) + 2] = 255;
				}
#endif
				v3fsub(&v[0], v[0], cv);

				v3fmul(&v[0], v[0], 0.1f/(NPX/10));
				v[0] = toxy(v[0], WX, WY, pv);

				if (z + 1 < NPX)
				{
					v[1] = p[(z + 1)*NPX*NPX + y*NPX + x];
					v3fsub(&v[1], v[1], cv);
					v3fmul(&v[1], v[1], 0.1f/(NPX/10));
					v[1] = toxy(v[1], WX, WY, pv);
					linexy(v[0], v[1], WX, WY, im, 1.0f*z/NPX, 0.5, 0.5);
					//glVertexPointer(3, GL_FLOAT, 0, v);
					//glDrawArrays(GL_LINES, 0, 2);
				}
				if (x + 1 < NPX)
				{
					v[1] = p[z*NPX*NPX + y*NPX + x+1];
					v3fsub(&v[1], v[1], cv);
					v3fmul(&v[1], v[1], 0.1f/(NPX/10));
					v[1] = toxy(v[1], WX, WY, pv);
					linexy(v[0], v[1], WX, WY, im, 0.5, 1.0f*x / NPX, 0.5);
					//glVertexPointer(3, GL_FLOAT, 0, v);
					//glDrawArrays(GL_LINES, 0, 2);
				}
				if (y + 1 < NPX)
				{
					v[1] = p[z*NPX*NPX + (y+1)*NPX + x];
					v3fsub(&v[1], v[1], cv);
					v3fmul(&v[1], v[1], 0.1f/(NPX/10));
					v[1] = toxy(v[1], WX, WY, pv);
					linexy(v[0], v[1], WX, WY, im, 0.5, 0.5, 1.0f*y / NPX);
					//glVertexPointer(3, GL_FLOAT, 0, v);
					//glDrawArrays(GL_LINES, 0, 2);
				}
			}
		}
	}
#endif

	//v[0].x = 0;
	//v[0].y = 0;
	//v[0].z = 0;
	//v[1] = v[2] = v[0];

	//glVertexPointer(3, GL_FLOAT, 0, v);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//v[0].x = -5 * 10;
	//v[0].y = 0;
	//v[0].z = 0;
	//v[1].x = 5 * 10;
	//v[1].y = 0;
	//v[1].z = 0;
	//linexy(v[0], v[1], WX, WY, im, 1, 1, 1);

	sprintf(fm, "%d.png", ff);
	savepng(fm, &pm);
	free(im);


	for (i = 0; i < NP; ++i)
	{
		v3fsub(&dv, cv, p[i]);
		d = mag3f(dv);
		v3fmul(&vv, dv, 0.0001 / (d*d*d));
		v3fadd(&p[i], p[i], vv);
		//v3fadd(&pvv[i], pvv[i], vv);

		v3fsub(&dv, cv2, p[i]);
		d = mag3f(dv);
		v3fmul(&vv, dv, 0.0001 / (d*d*d));
		v3fadd(&p[i], p[i], vv);
		//v3fadd(&pvv[i], pvv[i], vv);

		v3fadd(&p[i], p[i], pvv[i]);
	}

	//endsh();
}

void drawscenedepth()
{
}

void makefbo(unsigned int* rendertex, unsigned int* renderrb, unsigned int* renderfb, unsigned int* renderdepthtex, int w, int h)
{
	/* OpenGL 1.4 way */
	GLenum DrawBuffers[2];

	glGenTextures(1, rendertex);
	glBindTexture(GL_TEXTURE_2D, *rendertex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, renderdepthtex);
	glBindTexture(GL_TEXTURE_2D, *renderdepthtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, renderfb);
	glBindFramebuffer(GL_FRAMEBUFFER, *renderfb);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *renderdepthtex, 0);

	DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
	DrawBuffers[1] = GL_DEPTH_ATTACHMENT;
	glDrawBuffers(1, DrawBuffers);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		errm("Error", "Couldn't create framebuffer for render.");
		return;
	}
}

void delfbo(unsigned int* rendertex, unsigned int* renderrb, unsigned int* renderfb, unsigned int* renderdepthtex)
{
	/* delete resources */
	glDeleteTextures(1, rendertex);
	glDeleteTextures(1, renderdepthtex);
	/* Bind 0, which means render to back buffer, as a result, fb is unbound */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, renderfb);
	CHECKGL();
}

void draw()
{
	float white[4] = {1,1,1,1};
	float frame[4] = {0,0,(float)g_width,(float)g_height};
	wg *gui;
	mf proj, view, model, mvinv;
	float lpos[3], ldir[3];

	gui = (wg*)&g_gui;

	/* TODO leave as float for now then use fixmath int's */

	flatview(g_width, g_height, 1, 1, 1, 1);
	glDisable(GL_DEPTH_TEST);

	//drawim(g_tex[0].texname,
	//	0,0,100,100, 
	//	0,0,1,1,
	//	frame);
#if 01
	proj = pproj(45.0f,
		(float)g_width/(float)g_height,
		1.0f, MAX_DISTANCE);

	view = lookat(0,0,0,
		0,0,-100,
		0,1,0);

	mfreset(&model);

	drawscene(proj.matrix, 
		view.matrix, 
		model.matrix, 
		mvinv.matrix, 
		lpos, ldir);
#endif

	drawt(MAINFONT16, frame, frame, "Test",
	white, 0, -1, dfalse, dfalse);

	wgframeup(gui);
	wgdraw(gui);

	endsh();
	glEnable(GL_DEPTH_TEST);
}

void loadcfg()
{
	lnode *rit; /* v2i */
	v2i *rp;
	int w, h;
	char cfgfull[DMD_MAX_PATH+1];
	char line[128];
	char key[128];
	char act[128];
	FILE *fp;
	float valuef;
	int valuei;
	dbool valueb;
	//int i;

	enumdisp();

	if(g_ress.size)
	{
		rit = g_ress.head;
		rp = (v2i*)rit->data;
		g_selres = *rp;
	}
	else
	{
		SDL_GL_GetDrawableSize(g_win, &w, &h);

		g_selres.x = w;
		g_selres.y = h;
	}

	for(rit=g_ress.head; rit; rit=rit->next)
	{
		/* below acceptable height? */
		if(g_selres.y < 480)
		{
			rp = (v2i*)rit->data;

			if(rp->y > g_selres.y &&
				rp->x > rp->y)
			{
				g_selres = *(v2i*)rit->data;
			}
		}
		/* already of acceptable height? */
		else
		{
			rp = (v2i*)rit->data;
			//get smallest acceptable resolution
			if(rp->x < g_selres.y &&
				rp->x > rp->y)
			{
				g_selres = *(v2i*)rit->data;
			}

			break;
		}
	}

	//SwitchLang(LANG_ENG);

	fullwrite(CFGFILE, cfgfull);

	fp = fopen(cfgfull, "r");

	if(!fp)
		return;

	while(!feof(fp))
	{
		fgets(line, 127, fp);

		if(strlen(line) > 127)
			continue;

		act[0] = 0;
		key[0] = 0;

		if(sscanf(line, "%s %s", key, act) < 2)
			continue;

		sscanf(act, "%f", &valuef);
		valuei = (int)valuef;
		valueb = (dbool)valuef;

		if(strcmp(key, "fullscreen") == 0)					g_fs = valueb;
		else if(strcmp(key, "client_width") == 0)			g_width = g_selres.x = valuei;
		else if(strcmp(key, "client_height") == 0)			g_height = g_selres.y = valuei;
		else if(strcmp(key, "screen_bpp") == 0)				g_bpp = valuei;
//		else if(strcmp(key, "volume") == 0)					SetVol(valuei);
//		else if(strcmp(key, "language") == 0)				SwitchLang(GetLang(act));
	}

	fclose(fp);
}

void loadname()
{
	char cfgfull[DMD_MAX_PATH+1];
	FILE *fp;

	fullwrite("name.txt", cfgfull);
	fp = fopen(cfgfull, "r");

	if(!fp)
	{
		//GenName(g_name);
		sprintf(g_name, "User%d", (int)(rand()%1000));
		return;
	}

	fgets(g_name, MAXNAME, fp);
	fclose(fp);
}

void writecfg()
{
	char cfgfull[DMD_MAX_PATH+1];
	FILE* fp = fopen(cfgfull, "w");
	fullwrite(CFGFILE, cfgfull);
	if(!fp)
		return;
	fprintf(fp, "fullscreen %d \r\n\r\n", g_fs ? 1 : 0);
	fprintf(fp, "client_width %d \r\n\r\n", g_selres.x);
	fprintf(fp, "client_height %d \r\n\r\n", g_selres.y);
	fprintf(fp, "screen_bpp %d \r\n\r\n", g_bpp);
	//fprintf(fp, "volume %d \r\n\r\n", g_volume);
	//fprintf(fp, "language %s\r\n\r\n", g_lang);
	fclose(fp);
}

void writename()
{
	char cfgfull[DMD_MAX_PATH+1];
	FILE* fp = fopen(cfgfull, "w");
	fullwrite("name.txt", cfgfull);
	if(!fp)
		return;
	fprintf(fp, "%s", g_name);
	fclose(fp);
}

/* Define the function to be called when ctrl-c (SIGINT) signal is sent to process */
void sigcall(int signum)
{
	exit(0);
}

void appinit()
{
	char msg[128];
	//SDL_version compile_version;
	//SDL_version *link_version;
	//int flags;
	//int initted;
	char full[DMD_MAX_PATH+1];

	fullpath("log.txt", full);
	g_applog = fopen(full, "wb");

#ifdef PLAT_LINUX
	signal(SIGINT, sigcall);
#endif

	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		sprintf(msg, "SDL_Init: %s\n", SDL_GetError());
		errm("Error", msg);
	}

#if 0
	if(SDLNet_Init() == -1)
	{
		sprintf(msg, "SDLNet_Init: %s\n", SDLNet_GetError());
		errm("Error", msg);
	}
#endif

#if 0
	link_version=(SDL_version*)Mix_Linked_Version();
	SDL_MIXER_VERSION(&compile_version);
	printf("compiled with SDL_mixer version: %d.%d.%d\n",
		compile_version.major,
		compile_version.minor,
		compile_version.patch);
	printf("running with SDL_mixer version: %d.%d.%d\n",
		link_version->major,
		link_version->minor,
		link_version->patch);

	// load support for the OGG and MOD sample/music formats
	flags=MIX_INIT_OGG|MIX_INIT_MP3;
	initted=Mix_Init(flags);
	if( (initted & flags) != flags)
	{
		sprintf(msg, "Mix_Init: Failed to init required ogg and mod support!\nMix_Init: %s", Mix_GetError());
		/* errm("Error", msg); */
	}

	if(SDL_Init(SDL_INIT_AUDIO)==-1) {
		sprintf(msg, "SDL_Init: %s\n", SDL_GetError());
		errm("Error", msg);
	}
	/* show 44.1KHz, signed 16bit, system byte order,
	      stereo audio, using 1024 byte chunks */
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1)
	{
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		errm("Error", msg);
	}

	Mix_AllocateChannels(SCHANS);
#endif

	if(!g_applog)
		openlog("log.txt", APPVER);

	srand((unsigned int)getticks());

	vinit(&g_texload, sizeof(textoload));

	/* TODO c-style inits, not constructors */
	loadcfg();
	loadname();
//	MapKeys();
}

void appdeinit()
{
//	lnode *cit;
	wgg* gui;
	unsigned __int64 start;

//	endsess();
//	FreeMap();

	gui = &g_gui;
	wgfree((wg*)gui);

	breakwin(APPTIT);

//	for(cit=g_cn.head; cit; cit=cit->next)
//	{
//		Disconnect((NetConn*)cit->data);
//	}

	start = getticks();
	/* After quit, wait to send out quit packet to make sure host/clients recieve it. */
	while (getticks() - start < QUIT_DELAY)
	{
//		if(NetQuit())
//			break;
//		if(g_sock)
//			UpdNet();
	}

//	if(g_sock)
//	{
//		SDLNet_UDP_Close(g_sock);
//		g_sock = NULL;
//	}

//	lfree(&g_cn);

//	FreeSounds();
	//Mix_CloseAudio();
	//Mix_Quit();
	//SDLNet_Quit();
	SDL_Quit();
}

int evproc(void *userdata, SDL_Event *e)
{
	wg *gui;
	inev ie;
	v2i old;

	gui = (wg*)&g_gui;

	ie.intercepted = dfalse;
	ie.curst = CU_DEFAULT;

	switch(e->type)
	{
		case SDL_QUIT:
			g_quit = dtrue;
			break;
		case SDL_KEYDOWN:
			ie.type = INEV_KEYDOWN;
			ie.key = e->key.keysym.sym;
			ie.scancode = e->key.keysym.scancode;
			CHECKGL();

			/* Handle copy */
			if( e->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
			{
				ie.type = INEV_COPY;
			}
			/* Handle paste */
			if( e->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
			{
				ie.type = INEV_PASTE;
			}
			/* Select all */
			if( e->key.keysym.sym == SDLK_a && SDL_GetModState() & KMOD_CTRL )
			{
				ie.type = INEV_SELALL;
			}

			CHECKGL();
			wgin(gui, &ie);
			CHECKGL();

			if(!ie.intercepted)
				g_keys[e->key.keysym.scancode] = dtrue;

			g_keyin = ie.intercepted;
			break;
		case SDL_KEYUP:
			ie.type = INEV_KEYUP;
			ie.key = e->key.keysym.sym;
			ie.scancode = e->key.keysym.scancode;

			CHECKGL();
			wgin(gui, &ie);
			CHECKGL();

			if(!ie.intercepted)
				g_keys[e->key.keysym.scancode] = dfalse;

			g_keyin = ie.intercepted;
			break;
		case SDL_TEXTINPUT:
			/* UTF8 */
			ie.type = INEV_TEXTIN;
			strcpy(ie.text, e->text.text);

			CHECKGL();
			wgin(gui, &ie);
			CHECKGL();
			break;

		case SDL_MOUSEWHEEL:
			ie.type = INEV_MOUSEWHEEL;
			ie.amount = e->wheel.y;

			CHECKGL();
				wgin(gui, &ie);
			CHECKGL();
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (e->button.button)
			{
			case SDL_BUTTON_LEFT:
				g_mousekeys[MOUSE_LEFT] = dtrue;

				ie.type = INEV_MOUSEDOWN;
				ie.key = MOUSE_LEFT;
				ie.amount = 1;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();

				g_keyin = ie.intercepted;
				break;
			case SDL_BUTTON_RIGHT:
				g_mousekeys[MOUSE_RIGHT] = dtrue;

				ie.type = INEV_MOUSEDOWN;
				ie.key = MOUSE_RIGHT;
				ie.amount = 1;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();
				break;
			case SDL_BUTTON_MIDDLE:
				g_mousekeys[MOUSE_MIDDLE] = dtrue;

				ie.type = INEV_MOUSEDOWN;
				ie.key = MOUSE_MIDDLE;
				ie.amount = 1;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (e->button.button)
			{
			case SDL_BUTTON_LEFT:
				g_mousekeys[MOUSE_LEFT] = dfalse;

				ie.type = INEV_MOUSEUP;
				ie.key = MOUSE_LEFT;
				ie.amount = 1;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();
				break;
			case SDL_BUTTON_RIGHT:
				g_mousekeys[MOUSE_RIGHT] = dfalse;

				ie.type = INEV_MOUSEUP;
				ie.key = MOUSE_RIGHT;
				ie.amount = 1;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();
				break;
			case SDL_BUTTON_MIDDLE:
				g_mousekeys[MOUSE_MIDDLE] = dfalse;

				ie.type = INEV_MOUSEUP;
				ie.key = MOUSE_MIDDLE;
				ie.amount = 1;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();
				break;
			}
			break;
		case SDL_MOUSEMOTION:

			if(g_mouseout)
			{
				g_mouseout = dfalse;
			}

			old = g_mouse;

			if(mousepos())
			{
				ie.type = INEV_MOUSEMOVE;
				ie.x = g_mouse.x;
				ie.y = g_mouse.y;
				ie.dx = g_mouse.x - old.x;
				ie.dy = g_mouse.y - old.y;

				CHECKGL();
				wgin(gui, &ie);
				CHECKGL();

				g_curst = ie.curst;
			}
			break;
	}

	return 0;
}

void evloop()
{
	SDL_Event e;

	CHECKGL();
	while (!g_quit)
	{
		CHECKGL();
		while (SDL_PollEvent(&e))
		{
			evproc(NULL, &e);
		}
		CHECKGL();

		if(g_quit)
			break;

		if ( !g_bg &&
			( (g_appmode == APPMODE_LOADING || g_appmode == APPMODE_RELOADING) || drawnext() ) )
		{
			CHECKGL();

			calcdrawrate();

			CHECKGL();
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

			draw();
			CHECKGL();
			SDL_GL_SwapWindow(g_win);
			CHECKGL();

			CHECKGL();
		}

		if((g_appmode == APPMODE_LOADING || g_appmode == APPMODE_RELOADING) || upnext() )
		{
			calcuprate();
			update();
		}

		CHECKGL();
	}
}

#ifdef USESTEAM
//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
	// if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
	// if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
#ifdef PLAT_WIN
	::OutputDebugString( pchDebugText );
#endif

	if(!g_applog)
		openlog("log.txt", APPVER);

	Log(pchDebugText);


	if ( nSeverity >= 1 )
	{
		// place to set a breakpoint for catching API errors
		int x = 3;
		x = x;
	}
}
#endif

void appmain()
{
	//*((int*)0) = 0;

#ifdef USESTEAM

	if ( SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ) )
	{
		// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
		// local Steam client and also launches this game again.

		// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
		// removed steaappid.txt from the game depot.

		return;
	}

	// appinit Steam CEG
	if ( !Steamworks_InitCEGLibrary() )
	{
#ifdef PLAT_WIN
		OutputDebugString( "Steamworks_InitCEGLibrary() failed\n" );
#endif
		errm( "Fatal Error", "Steam must be running to play this game (InitDrmLibrary() failed).\n" );
		return;
	}

	// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
	// You don't necessarily have to though if you write your code to check whether all the Steam
	// interfaces are NULL before using them and provide alternate paths when they are unavailable.
	//
	// This will also load the in-game steam overlay dll into your process.  That dll is normally
	// injected by steam when it launches games, but by calling this you cause it to always load,
	// even when not launched via steam.
	if ( !SteamAPI_Init() )
	{
#ifdef PLAT_WIN
		OutputDebugString( "SteamAPI_Init() failed\n" );
#endif
		errm( "Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n" );
		return;
	}

	// set our debug handler
	SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

#endif

	appinit();

	makewin(APPTIT);

	//SDL_ShowCursor(dfalse);
	loadsysres();
	queuesimres();
	makewg();

	evloop();

	appdeinit();
	//SDL_ShowCursor(dtrue);
}

dbool runops(const char* cmdline)
{
	if(strcmp(cmdline, "") == 0)
	{
//		strcpy(g_startmap, "");

		return dfalse;
	}
	else
	{
		/*
		TODO c90
		std::string cmdlinestr(cmdline);
		std::string find("+devmap ");
		int found = cmdlinestr.rfind(find);

		if(found != std::string::npos)
		{
			strcpy(g_startmap, "");

			startmap = cmdlinestr.psubstr(found+find.length(), cmdlinestr.length()-found-find.length());

			fprintf(g_applog, "%s\r\n", cmdline);
			fprintf(g_applog, "%s\r\n", startmap);

			//LoadMap(startmap.c_str());
			g_startmap = startmap;
		}
		*/
	}

	return dfalse;
}

#ifdef PLAT_WIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
  //  SDL_SetMainReady();

#ifdef PLAT_WIN
	g_hinst = hInstance;
#endif

#ifdef PLAT_WIN
	//runops(lpCmdLine);
#else
	char cmdline[DMD_MAX_PATH+124];
	cmdline[0] = 0;
	for(int ai=0; ai<argc; ai++)
	{
		strcat(cmdline, argv[ai]);

		if(ai+1<argc)
			strcat(cmdline, " ");
	}
	runops(cmdline);
#endif

#ifdef PLAT_WIN
	if ( IsDebuggerPresent() )
	{
		// We don't want to mask exceptions (or report them to Steam!) when debugging.
		// If you would like to step through the exception handler, attach a debugger
		// after running the game outside of the debugger.	

		appmain();
		return 0;
	}
#endif

#ifdef PLAT_WIN
#ifdef USESTEAM
	_set_se_translator( MiniDumpFunction );

	try  // this try block allows the SE translator to work
	{
#endif
#endif
		appmain();
#ifdef PLAT_WIN
#ifdef USESTEAM
	}
	catch( ... )
	{
		return -1;
	}
#endif
#endif

	return 0;
}
