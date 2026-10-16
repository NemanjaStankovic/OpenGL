#include "StdAfx.h"
#include "GLRenderer.h"
#include "DImage.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#define toRad 3.14/180
#define toDeg 180/3.14
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	alpha = 45*3.14/180;
	beta = 45 * 3.14 / 180;
	udaljenje = 10;
	kameraX = udaljenje * cos(alpha)*cos(beta);
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
	glClearColor(1.0, 1.0, 1.0, 0.0);			//dodato
	back=LoadTexture("res/back.jpg");
	bot=LoadTexture("res/bot.jpg");
	front = LoadTexture("res/front.jpg");
	left = LoadTexture("res/left.jpg");
	right = LoadTexture("res/right.jpg");
	ShipT1 = LoadTexture("res/ShipT1.png");
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
	gluLookAt(kameraX, kameraY, kameraZ, 0.0, 2.5, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	crtajKoordinate();
	glRotatef(90,1,0,0);
	DrawShip();
	DrawSpaceCube(20);
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
	double aspect = (double)w/(double)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, aspect, 0.1, 100);               //podesava se FOV=50
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &back);
	glDeleteTextures(1, &bot);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &ShipT1);
	glDeleteTextures(1, &top);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::crtajKoordinate()
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(10.0, 0.0, 0.0);
		
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 10.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 10.0);
	}
	glEnd();
}

void CGLRenderer::DrawShip()
{
	double a1 = atan2(2.15, 5.8);
	float rep = 3.0;
	//Gornji levo
	glPushMatrix();
	glPushMatrix();
	glRotatef(-4.75, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-a1*toDeg, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();
	glPushMatrix();
	glRotatef(4.75, 0, 1, 0);
	glRotatef(-13, 1, 0, 0);
	glRotatef(-a1 * toDeg, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();
	//glPushMatrix();
	glRotatef(-a1*toDeg, 0, 0, 1);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	glPushMatrix();
	glRotatef(-4.75, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-a1*toDeg, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();
	glPushMatrix();
	glRotatef(4.75, 0, 1, 0);
	glRotatef(-13, 1, 0, 0);
	glRotatef(-a1 * toDeg, 0, 0, 1);
	DrawTriangle(5.8, 2.15, rep);
	glPopMatrix();
	glRotatef(-a1*toDeg, 0, 0, 1);
	glPopMatrix();
}

void CGLRenderer::DrawSpaceCube(double a)
{
	glEnable(GL_TEXTURE_2D);
	//gore
	glBindTexture(GL_TEXTURE_2D, top);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 1.0);
		glVertex3f(a / 2, a / 2, a / 2);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-a / 2, a / 2, a / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-a / 2, a / 2, -a / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(a / 2, a / 2, -a / 2);
	}
	glEnd();
	//dole
	glBindTexture(GL_TEXTURE_2D, bot);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-a / 2, -a / 2, -a / 2);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-a / 2, -a / 2, a / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(a / 2, -a / 2, a / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(a / 2, -a / 2, -a / 2);
	}
	glEnd();
	//leva
	glBindTexture(GL_TEXTURE_2D, left);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 1.0);
		glVertex3f(a / 2, a / 2, a / 2);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(a / 2, -a / 2, a / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-a / 2, -a / 2, a / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(-a / 2, a / 2, a / 2);
	}
	glEnd();
	//prednja
	glBindTexture(GL_TEXTURE_2D, front);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-a / 2, a / 2, a / 2);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-a / 2, -a / 2, a / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-a / 2, -a / 2, -a / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(-a / 2, a / 2, -a / 2);
	}
	glEnd();
	//pozadi
	glBindTexture(GL_TEXTURE_2D, back);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 1.0);
		glVertex3f(a / 2, a / 2, a / 2);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(a / 2, -a / 2, a / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(a / 2, -a / 2, -a / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(a / 2, a / 2, -a / 2);
	}
	glEnd();
	//desna
	glBindTexture(GL_TEXTURE_2D, right);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-a / 2, a / 2, -a / 2);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-a / 2, -a / 2, -a / 2);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(a / 2, -a / 2, -a / 2);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(a / 2, a / 2, -a / 2);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	
}

void CGLRenderer::DrawTriangle(float d1, float d2, float rep)
{
	double ugao = atan2(d2, d1);         //ugao izmedju (0,0) i (d1,d2);
	double d3 = sqrt(d1 * d1 + d2 * d2);
	double y = cos(ugao) * d1 / d3;				//zasto /d3 ?????
	double x = sin(ugao) * d1 / d3;
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ShipT1);
	glLineWidth(2.0);
	glBegin(GL_TRIANGLES);
	{
		glTexCoord2f(0.5*rep, 0.0);
		glVertex3f(0, 0, 0);

		glTexCoord2f((x + 0.5) * rep, y * rep);          //kad predje 1 duplira se slika i dobije se nesto sto lici na cetvorostruku sliku na koju je nacrtan trougao isto kao kad
														//je bila jednostruka
		glVertex3f(d1, 0, 0);

		glTexCoord2f(0.5 * rep, 1.0 * rep);
		glVertex3f(d1, d2, 0);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
