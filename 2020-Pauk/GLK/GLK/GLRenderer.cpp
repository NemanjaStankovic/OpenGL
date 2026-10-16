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
	alpha = 45*3.14/180;
	beta = 45 * 3.14 / 180;
	pogledUgao = 0;
	daljina = 50;
	kameraX = daljina * cos(alpha) * cos(beta);
	kameraY = daljina * sin(alpha);
	kameraZ = daljina * cos(alpha) * sin(beta);
	pogledY = 500 * sin(pogledUgao);
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
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	spider = LoadTexture("res/spider.png");
	front = LoadTexture("res/front.jpg");
	left = LoadTexture("res/left.jpg");
	right = LoadTexture("res/right.jpg");
	back = LoadTexture("res/back.jpg");
	top = LoadTexture("res/top.jpg");
	bot = LoadTexture("res/bot.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(kameraX, kameraY, kameraZ, 0, pogledY, 0, 0, 1, 0);
	DrawAxes();
	DrawEnvCube(100);
	glTranslatef(0, 4, 0);
	//DrawCone(5, 5, 5, 0, 0, 0);
	//DrawSphere(5, 5, 0, 0, 0);
	DrawSpider();
	glFlush();
	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, (double)w / (double)h, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &spider);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &back);
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(50, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 50, 0);
		
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 50);
	}
	glEnd();

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

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	return texID;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, front);
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
		glVertex3f(-a / 2, a, a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, right);
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
	glBindTexture(GL_TEXTURE_2D, back);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0);
		glVertex3f(a / 2, a, -a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(a / 2, a, a / 2);
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
		glVertex3f(-a / 2, a, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(-a / 2, a, -a / 2);
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
		glVertex3f(-a / 2, 0, a / 2);
		glTexCoord2f(0, 1);
		glVertex3f(a / 2, 0, a / 2);
		glTexCoord2f(1, 1);
		glVertex3f(a / 2, 0, -a / 2);
		glTexCoord2f(1, 0);
		glVertex3f(-a / 2, 0, -a / 2);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	//glColor3f(0, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spider);
	float alfaKorak = 180 / nSeg;  //36
	float betaKorak = 180 / (2 * nSeg);  //18
	float alfa2 = 90, beta2 = 0;
	while (alfa2 > -90)
	{
		glBegin(GL_QUAD_STRIP);
		while (beta2 <= 360)
		{
			float x = r * cos(alfa2*3.14/180) * cos(beta2 * 3.14 / 180);
			float y = r * sin(alfa2 * 3.14 / 180);
			float z = r * cos(alfa2 * 3.14 / 180) * sin(beta2 * 3.14 / 180);
			glTexCoord2f((x/r)*texR+texU, (z/r)*texR+texV); 
			glVertex3f(x, y, z);
			
			alfa2 -= alfaKorak;
			x = r * cos(alfa2 * 3.14 / 180) * cos(beta2 * 3.14 / 180);
			y = r * sin(alfa2 * 3.14 / 180);
			z = r * cos(alfa2 * 3.14 / 180) * sin(beta2 * 3.14 / 180);
			glTexCoord2f((x / r) * texR + texU, (z / r) * texR + texV);
			glVertex3f(x, y, z);
			alfa2 += alfaKorak;
			beta2 += betaKorak;
		}
		glEnd();
		beta2 = 0;
		alfa2 -= alfaKorak;
	}
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spider);
	double alfa = 0;
	double alfaKorak = 2 * 3.14 / nSeg;
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(texU, texV);
	glVertex3f(0, h, 0);
	while (alfa <= 2 * 3.14)
	{
		float x = r * cos(alfa);
		float z = r * sin(alfa);
		glTexCoord2f(x/r* texR +texU, z/r* texR +texV);
		glVertex3f(x,0, z);
		alfa += alfaKorak;
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	DrawSphere(r, nSeg, 0.75, 0.75, 0.25);
	DrawCone(r, h, nSeg, 0.25, 0.25, 0.25);
}

void CGLRenderer::DrawLeg()
{
	DrawLegSegment(1,10,5);
	glTranslatef(0, 10, 0);
	glRotatef(85, 1, 0, 0);
	DrawLegSegment(1,15,5);
}

void CGLRenderer::DrawSpiderBody()
{
	//glTranslatef(0, 5, 0);
	glPushMatrix();
	glScalef(1, 0.5, 1);
	DrawSphere(3, 10, 0.25, 0.25, 0.24);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(7.5, 0, 0);
	glScalef(1, 0.8, 1);
	DrawSphere(5, 10, 0.25, 0.25, 0.24);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-4.5, 0, 0);
	glScalef(1, 0.5, 1);
	DrawSphere(2, 10, 0.75, 0.25, 0.24);
	glPopMatrix();
}

void CGLRenderer::DrawSpider()
{
	DrawSpiderBody();
	//glRotatef(45, 1, 0, 0);
	glPushMatrix();
	glRotatef(-45, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(-45+30, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(-45+60, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(-45+90, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	//
	glPushMatrix();
	glRotatef(-135, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(-135 - 30, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(-135 - 60, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
	glPushMatrix();
	glRotatef(-135 - 90, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	DrawLeg();
	glPopMatrix();
}



