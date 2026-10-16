#include "StdAfx.h"
#include "GLRenderer.h"
#include "DImage.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	alpha = 45 * 3.14 / 180;
	beta = 45 * 3.14 / 180;
	kabinaRot = 0;
	prvaRukaRot = 0;
	drugaRukaRot = 0;
	viljuskaRot = 0;
	udaljenje = 10;
	kameraX = udaljenje * cos(alpha) * cos(beta);
	kameraY = udaljenje * sin(alpha);
	kameraZ = udaljenje * cos(alpha) * sin(beta);
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
	back = LoadTexture("res/back.jpg");
	excavator = LoadTexture("res/excavator.png");
	bot = LoadTexture("res/bot.jpg");
	front = LoadTexture("res/front.jpg");
	left = LoadTexture("res/left.jpg");
	right = LoadTexture("res/right.jpg");
	top = LoadTexture("res/top.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //dodato
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();
	gluLookAt(kameraX, kameraY, kameraZ, 0.0, 5.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	crtajLinke();
	DrawEnvCube(100);
	DrawExcavator();
	//---------------------------------
	SwapBuffers(pDC->m_hDC);
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
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(),
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);  //dodato
	double aspect = (double)w / (double)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(55, aspect, 0.1, 140);               //podesava se FOV=55
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

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	
	glColor3f(1.0, 1.0, 1.0);
	glEnd();
}

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);
	glBegin(GL_TRIANGLE_FAN);
	{
		for (int i = 0; i < n; i++)
		{
			glTexCoord2f(texCoords[i].x, texCoords[i].y);
			glVertex3f(points[i].x, points[i].y, -2);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glColor3f(r, g, b);
	DrawPolygon(points, texCoords, n);
	glColor3f(r, g, b);
	glBegin(GL_QUAD_STRIP);
	{
		for (int i = 0; i < n; i++)
		{
			glVertex3f(points[i].x, points[i].y, -zh/2);
			glVertex3f(points[i].x, points[i].y, zh/2);
		}
		glVertex3f(points[0].x, points[0].y, -zh/2);
		glVertex3f(points[0].x, points[0].y, zh/2);
	}
	glEnd();
	glPushMatrix();
	glTranslatef(0, 0, zh);
	DrawPolygon(points, texCoords, n);
	glPopMatrix();
}

void CGLRenderer::DrawBase()
{
	//glColor3f(0.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(0, 13.0 / 16.0);
		glVertex3f(-8 * 0.5, 0.5, 2.5); //skroz levo tacka gornja pa ide ukoso u pravcu kazaljke
		glTexCoord2f(1.0/16, 12.0 / 16);
		glVertex3f(-7 * 0.5, 1.0, 2.5);
		glTexCoord2f(15.0/16, 12.0/ 16);
		glVertex3f(7 * 0.5, 1.0, 2.5);
		glTexCoord2f(16.0/16, 13.0 / 16.0);
		glVertex3f(8 * 0.5, 0.5, 2.5);
		glTexCoord2f(16/16, 15.0/16);
		glVertex3f(8 * 0.5, -0.5, 2.5);
		glTexCoord2f(15.0/16, 1);
		glVertex3f(7 * 0.5, -1.0, 2.5);
		glTexCoord2f(1.0/16, 1);
		glVertex3f(-7 * 0.5, -1.0, 2.5);
		glTexCoord2f(0, 15.0/16);
		glVertex3f(-8 * 0.5, -0.5, 2.5);
	}
	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_QUAD_STRIP);
	{
		glVertex3f(-8 * 0.5, 0.5, 2.5);
		glVertex3f(-8 * 0.5, 0.5, -2.5);
		glVertex3f(-7 * 0.5, 1.0, 2.5);
		glVertex3f(-7 * 0.5, 1.0, -2.5);
		glVertex3f(7 * 0.5, 1.0, 2.5);
		glVertex3f(7 * 0.5, 1.0, -2.5);
		glVertex3f(8 * 0.5, 0.5, 2.5);
		glVertex3f(8 * 0.5, 0.5, -2.5);
		glVertex3f(8 * 0.5, -0.5, 2.5);
		glVertex3f(8 * 0.5, -0.5, -2.5);
		glVertex3f(7 * 0.5, -1.0, 2.5);
		glVertex3f(7 * 0.5, -1.0, -2.5);
		glVertex3f(-7 * 0.5, -1.0, 2.5);
		glVertex3f(-7 * 0.5, -1.0, -2.5);
		glVertex3f(-8 * 0.5, -0.5, 2.5);
		glVertex3f(-8 * 0.5, -0.5, -2.5);
		glVertex3f(-8 * 0.5, 0.5, 2.5);
		glVertex3f(-8 * 0.5, 0.5, -2.5);
	}
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(0, 13.0 / 16.0);
		glVertex3f(-8 * 0.5, 0.5, -2.5); //skroz levo tacka gornja pa ide ukoso u pravcu kazaljke
		glTexCoord2f(1.0 / 16, 12.0 / 16);
		glVertex3f(-7 * 0.5, 1.0, -2.5);
		glTexCoord2f(15.0 / 16, 12.0 / 16);
		glVertex3f(7 * 0.5, 1.0, -2.5);
		glTexCoord2f(16.0 / 16, 13.0 / 16.0);
		glVertex3f(8 * 0.5, 0.5, -2.5);
		glTexCoord2f(16 / 16, 15.0 / 16);
		glVertex3f(8 * 0.5, -0.5, -2.5);
		glTexCoord2f(15.0 / 16, 1);
		glVertex3f(7 * 0.5, -1.0, -2.5);
		glTexCoord2f(1.0 / 16, 1);
		glVertex3f(-7 * 0.5, -1.0, -2.5);
		glTexCoord2f(0, 15.0 / 16);
		glVertex3f(-8 * 0.5, -0.5, -2.5);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawBody()
{
	POINTF* points = new POINTF[5];
	points[0].x = 2;
	points[0].y = 2;
	points[1].x = 2;
	points[1].y = -2;
	points[2].x = -2;
	points[2].y = -2;
	points[3].x = -2;
	points[3].y = 0;
	points[4].x = 0;
	points[4].y = 2;
	POINTF* kurac = new POINTF[5];
	kurac[0].x = 0.5;
	kurac[0].y = 0;
	kurac[1].x = 0.5;
	kurac[1].y = 0.5;
	kurac[2].x = 1.0;
	kurac[2].y = 0.5;
	kurac[3].x = 1.0;
	kurac[3].y = 0.25;
	kurac[4].x = 0.75;
	kurac[4].y = 0;
	DrawExtrudedPolygon(points, kurac, 5, 4, 0.96, 0.5, 0.12);
}

void CGLRenderer::DrawArm(double zh)
{
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(1.0 / 16, 8.0 / 16);
		glVertex3f(-0.5, -1.0, -zh/2);
		glTexCoord2f(4.0 / 16, 8.0 / 16);
		glVertex3f(1.0, -1.0, -zh / 2);
		glTexCoord2f(16 / 16, 9.0 / 16);
		glVertex3f(8, -0.5, -zh / 2);
		glTexCoord2f(1.0, 11.0 / 16);
		glVertex3f(8, 0.5, -zh / 2);
		glTexCoord2f(4.0 / 16, 12.0 / 16);
		glVertex3f(1.0, 1.0, -zh / 2);
		glTexCoord2f(1.0 / 16, 12.0 / 16);
		glVertex3f(-0.5, 1.0, -zh / 2);
		glTexCoord2f(0.0 / 16, 11.0 / 16);
		glVertex3f(-1.0, 0.5, -zh / 2);
		glTexCoord2f(0.0 / 16, 9.0 / 16);
		glVertex3f(-1.0, -0.5, -zh / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(0.96, 0.5, 0.12);

	glBegin(GL_QUAD_STRIP);
	{
		glVertex3f(-0.5, -1.0, zh / 2);
		glVertex3f(-0.5, -1.0, -zh / 2);
		glVertex3f(1.0, -1.0, zh / 2);
		glVertex3f(1.0, -1.0, -zh / 2);
		glVertex3f(8, -0.5, zh / 2);
		glVertex3f(8, -0.5, -zh / 2);
		glVertex3f(8, 0.5, zh / 2);
		glVertex3f(8, 0.5, -zh / 2);
		glVertex3f(1.0, 1.0, zh / 2);
		glVertex3f(1.0, 1.0, -zh / 2);
		glVertex3f(-0.5, 1.0, zh / 2);
		glVertex3f(-0.5, 1.0, -zh / 2);
		glVertex3f(-1.0, 0.5, zh / 2);
		glVertex3f(-1.0, 0.5, -zh / 2);
		glVertex3f(-1.0, -0.5, zh / 2);
		glVertex3f(-1.0, -0.5, -zh / 2);
		glVertex3f(-0.5, -1.0, zh / 2);
		glVertex3f(-0.5, -1.0, -zh / 2);
	}
	glEnd();
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	{
		glTexCoord2f(1.0 / 16, 8.0 / 16);
		glVertex3f(-0.5, -1.0, zh / 2);
		glTexCoord2f(4.0 / 16, 8.0 / 16);
		glVertex3f(1.0, -1.0, zh / 2);
		glTexCoord2f(16 / 16, 9.0 / 16);
		glVertex3f(8, -0.5, zh / 2);
		glTexCoord2f(1.0, 11.0 / 16);
		glVertex3f(8, 0.5, zh / 2);
		glTexCoord2f(4.0 / 16, 12.0 / 16);
		glVertex3f(1.0, 1.0, zh / 2);
		glTexCoord2f(1.0 / 16, 12.0 / 16);
		glVertex3f(-0.5, 1.0, zh / 2);
		glTexCoord2f(0.0 / 16, 11.0 / 16);
		glVertex3f(-1.0, 0.5, zh / 2);
		glTexCoord2f(0.0 / 16, 9.0 / 16);
		glVertex3f(-1.0, -0.5, zh / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void CGLRenderer::DrawFork()
{
	float kvadratic = 1.0 / 16;
	glColor3f(0.7, 0.7, 0.7);
	glBindTexture(GL_TEXTURE_2D, excavator);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	{

		glTexCoord2f(0, 1*kvadratic);
		glVertex3f(-1, +1.5, -0.25);
		glTexCoord2f(1*kvadratic, 0);
		glVertex3f(-0.5, +2, -0.25);
		glTexCoord2f(7 * kvadratic, 0 * kvadratic);
		glVertex3f(2.5, +2, -0.25);
		glTexCoord2f(8 * kvadratic, 1 * kvadratic);
		glVertex3f(3, 1.5, -0.25);
		glTexCoord2f(8 * kvadratic, 6 * kvadratic);
		glVertex3f(3, -1, -0.25);
		glTexCoord2f(0 * kvadratic, 6 * kvadratic);
		glVertex3f(-1, -1, -0.25);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_QUAD_STRIP);
	{
		glVertex3f(-1, +1.5, 0.25);
		glVertex3f(-1, +1.5, -0.25);
		glVertex3f(-0.5, +2, 0.25);
		glVertex3f(-0.5, +2, -0.25);
		glVertex3f(2.5, +2, 0.25);
		glVertex3f(2.5, +2, -0.25);
		glVertex3f(3, 1.5, 0.25);
		glVertex3f(3, 1.5, -0.25);
		glVertex3f(3, -1, 0.25);
		glVertex3f(3, -1, -0.25);
		glVertex3f(-1, -1, 0.25);
		glVertex3f(-1, -1, -0.25);
		glVertex3f(-1, +1.5, 0.25);
		glVertex3f(-1, +1.5, -0.25);
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(0, 1 * kvadratic);
		glVertex3f(-1, +1.5, 0.25);
		glTexCoord2f(1 * kvadratic, 0);
		glVertex3f(-0.5, +2, 0.25);
		glTexCoord2f(7 * kvadratic, 0 * kvadratic);
		glVertex3f(2.5, +2, 0.25);
		glTexCoord2f(8 * kvadratic, 1 * kvadratic);
		glVertex3f(3, 1.5, 0.25);
		glTexCoord2f(8 * kvadratic, 6 * kvadratic);
		glVertex3f(3, -1, 0.25);
		glTexCoord2f(0 * kvadratic, 6 * kvadratic);
		glVertex3f(-1, -1, 0.25);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawExcavator()
{
	glTranslatef(0, 1, 0);
	DrawBase();
	glTranslatef(0, 3, 0);
	glRotatef(180+kabinaRot, 0, 1, 0);
	DrawBody();
	glTranslatef(-1, 0, 2.5);
	glRotatef(90+prvaRukaRot, 0, 0, 1);
	DrawArm(1);
	glTranslatef(7, 0, 0);
	glRotatef(90+drugaRukaRot, 0, 0, 1);
	DrawArm(1.5);
	glTranslatef(7, 0, 0);
	glRotatef(180, 0, 1, 0);
	glRotatef(90+viljuskaRot, 0, 0, 1);
	DrawFork();
	//DrawFork();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, front);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(1, 0);
		glVertex3f(a / 2, a, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, a, -a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, back);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, a, a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, 0, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, 0, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, a, -a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, left);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, a, a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, 0, a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, a, a/ 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, left);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex3f(-a / 2, a, -a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(-a / 2, 0, -a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(a / 2, a, -a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bot);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, 0, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, 0, a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, top);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, a, a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, a, -a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, a, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, a, a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
