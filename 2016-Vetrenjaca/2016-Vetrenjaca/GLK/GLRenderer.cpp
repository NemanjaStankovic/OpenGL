#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	alpha = 0;
	beta =0;
	udaljenje = 10;
	kameraX = udaljenje * cos(alpha) * cos(beta) + 20;
	kameraY = udaljenje * sin(alpha)+5;
	kameraZ = udaljenje * cos(alpha) * sin(beta);
	ugaoVetrenjace = 0;
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(1.0, 1.0, 1.0, 0.0);
	env = LoadTexture("res/env.png");
	brick = LoadTexture("res/brick.png");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //dodato
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();
	gluLookAt(20, 5, 0, kameraX, kameraY, kameraZ, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	crtajLinke();
	DrawCube(100);
	DrawTube(3.5, 2.5, 10, 32);
	glTranslatef(0, 10, 0);
	DrawCone(3.8, 2, 32);
	glTranslatef(3.8, 0, 0);
	glRotatef(-90, 0, 0, 1);
	glRotatef(ugaoVetrenjace, 0, 1, 0);
	DrawPaddle(8, 1.5);
	glRotatef(90, 0, 1, 0);
	DrawPaddle(8, 1.5);
	glRotatef(90, 0, 1, 0);
	DrawPaddle(8, 1.5);
	glRotatef(90, 0, 1, 0);
	DrawPaddle(8, 1.5);
	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);  //dodato
	double aspect = (double)w / (double)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, aspect, 0.1, 140);               //podesava se FOV=55
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::crtajLinke()
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
	}
	glEnd();
}

void CGLRenderer::DrawCube(double a)
{
	float kockicaDesno = 1.0 / 4;
	float kockicaDole = 1.0 / 3;
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, env);
	//iza
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1 * kockicaDesno, 0);
		glVertex3f(-a / 2, a, a / 2);

		glTexCoord2f(1 * kockicaDesno, 1*kockicaDole);
		glVertex3f(-a / 2, 0, a / 2);

		glTexCoord2f(2 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, 0, -a / 2);

		glTexCoord2f(2 * kockicaDesno, 0 * kockicaDole);
		glVertex3f(-a / 2, a, -a / 2);
	}
	glEnd();
	//ispod
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, 0, a / 2);

		glTexCoord2f(1 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, 0, a / 2);

		glTexCoord2f(2 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, 0, -a / 2);

		glTexCoord2f(2 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, 0, -a / 2);
	}
	glEnd();
	//levo
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, a, a / 2);

		glTexCoord2f(1 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, 0, a / 2);

		glTexCoord2f(1 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, 0, a / 2);

		glTexCoord2f(0 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, a, a / 2);
	}
	glEnd();
	//desno
	glBegin(GL_QUADS);
	{
		glTexCoord2f(3* kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, a, -a / 2);

		glTexCoord2f(2 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, 0, -a / 2);

		glTexCoord2f(2 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, 0, -a / 2);

		glTexCoord2f(3 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, a, -a / 2);
	}
	glEnd();
	//gore
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1, 1 * kockicaDole);
		glVertex3f(-a / 2, a, a / 2);

		glTexCoord2f(3 * kockicaDesno, 1 * kockicaDole);
		glVertex3f(-a / 2, a, -a / 2);

		glTexCoord2f(3 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, a, -a / 2);

		glTexCoord2f(1, 2 * kockicaDole);
		glVertex3f(a / 2, a, a / 2);
	}
	glEnd();

	//napred
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1*kockicaDesno, 1 );
		glVertex3f(a / 2, a, a / 2);

		glTexCoord2f(1 * kockicaDesno, 2 * kockicaDole);
		glVertex3f( a / 2, 0, a / 2);

		glTexCoord2f(2 * kockicaDesno, 2 * kockicaDole);
		glVertex3f(a / 2, 0, -a / 2);

		glTexCoord2f(2*kockicaDesno, 1);
		glVertex3f(a / 2, a, -a / 2);
	}
	glEnd();


	glDisable(GL_TEXTURE_2D);
}
void CGLRenderer::DrawTube(double r1, double r2, double h, int n)
{
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brick);
	float korakCigla = 1.0 / n;
	float x, z;
	float alfa = 0;
	float korak = 2 * 3.14 / n;
	glBegin(GL_POLYGON);
	{
		while (alfa < 2*3.14)
		{
			x = r1 * cos(alfa);
			z = r1 * sin(alfa);
			glVertex3f(x, 0, z);
			alfa += korak;
		}
	}
	glEnd();
	alfa = 0;
	glBegin(GL_QUAD_STRIP);
	{
		while (alfa <= 2 * 3.14)
		{
			float x1 = r1 * cos(alfa);
			float z1 = r1 * sin(alfa);
			float x2 = r2 * cos(alfa);
			float z2 = r2 * sin(alfa);
			glTexCoord2f(alfa/2*3.14, 0);
			glVertex3f(x2, h, z2);
			glTexCoord2f(alfa / 2 * 3.14, 1);
			glVertex3f(x1, 0, z1);
			alfa += korak;

		}
	}
	glEnd();
	alfa = 0;
	glBegin(GL_POLYGON);
	{
		while (alfa < 2 * 3.14)
		{
			x = r2 * cos(alfa);
			z = r2 * sin(alfa);
			glVertex3f(x, h, z);
			alfa += korak;
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
void CGLRenderer::DrawCone(double r, double h, int n)
{
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, brick);
	float korakCigla = 1.0 / n;
	float x, z;
	float alfa = 0;
	float korak = 2 * 3.14159265359/ n;
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3f(0, h, 0);
		while (alfa <2 * 3.14159265359)
		{
			x = r * cos(alfa);
			z = r * sin(alfa);
			glVertex3f(x, 0, z);
			alfa += korak;
		}
		x = r * cos(2* 3.14159265359);
		z = r * sin(2 * 3.14159265359);
		glVertex3f(x, 0, z);
	}
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}
void CGLRenderer::DrawPaddle(double length, double width)
{
	glBegin(GL_POLYGON);
	{
		float pom = 0;
		glVertex3f(length, 0, -width/16);
		glVertex3f(0, 0, -width/16);
		glVertex3f(0, 0, width / 16);
		glVertex3f(1*length/8.0, 0, width / 16);
		glVertex3f(2*length / 8.0, 0, 15*width/16);
		glVertex3f(length, 0, 16*width/16);
	}
	glEnd();
}