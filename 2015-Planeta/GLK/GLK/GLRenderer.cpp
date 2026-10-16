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
	mesecUgao = 0;
	alpha = 0;
	beta = 45;
	daljina = 10;
	pogledY= daljina * sin(alpha);
	kameraX = daljina * cos(beta);
	kameraZ = daljina * sin(beta);
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
	glClearColor(1, 1, 1, 0);
	M0 = LoadTexture("res/M0.jpg");
	M1 = LoadTexture("res/M1.jpg");
	M2 = LoadTexture("res/M2.jpg");
	M3 = LoadTexture("res/M3.jpg");
	M4 = LoadTexture("res/M4.jpg");
	M5 = LoadTexture("res/M5.jpg");
	S0 = LoadTexture("res/S0.jpg");
	S1 = LoadTexture("res/S1.jpg");
	S2 = LoadTexture("res/S2.jpg");
	S3 = LoadTexture("res/S3.jpg");
	S4 = LoadTexture("res/S4.jpg");
	S5 = LoadTexture("res/S5.jpg");
	TSC0 = LoadTexture("res/TSC0.jpg");
	TSC1 = LoadTexture("res/TSC1.jpg");
	TSC2 = LoadTexture("res/TSC2.jpg");
	TSC3 = LoadTexture("res/TSC3.jpg");
	TSC4 = LoadTexture("res/TSC4.jpg");
	TSC5 = LoadTexture("res/TSC5.jpg");
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
	gluLookAt(kameraX, 0, kameraZ, 0, pogledY, 0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	crtajLinke();
	glPushMatrix();
	glRotatef(-90, 0, 0, 1);
	DrawEarth(5, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(50, 0, 0);
	glRotatef(-90, 0, 0, 1);
	glRotatef(mesecUgao, 1, 0, 0);
	DrawMoon(1, 20);
	glPopMatrix();
	glTranslatef(kameraX, 0, kameraZ);
	glRotatef(-90, 0, 0, 1);
	DrawSpace(200, 20);

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
	gluPerspective(40, aspect, 0.1, 500);               //podesava se FOV=55
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	glDeleteTextures(1, &M0);
	glDeleteTextures(1, &M1);
	glDeleteTextures(1, &M2);
	glDeleteTextures(1, &M3);
	glDeleteTextures(1, &M4);
	glDeleteTextures(1, &M5);
	glDeleteTextures(1, &S0);
	glDeleteTextures(1, &S1);
	glDeleteTextures(1, &S2);
	glDeleteTextures(1, &S3);
	glDeleteTextures(1, &S4);
	glDeleteTextures(1, &S5);
	glDeleteTextures(1, &TSC0);
	glDeleteTextures(1, &TSC1);
	glDeleteTextures(1, &TSC2);
	glDeleteTextures(1, &TSC3);
	glDeleteTextures(1, &TSC4);
	glDeleteTextures(1, &TSC5);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
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

void CGLRenderer::crtajLinke()
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
	}
	glEnd();
	glVertex3f(0, 0, 0);
}

void CGLRenderer::DrawPatch(double R, int n)
{
	float brojDelova = 1.0 / n;
	float s1 = 0, x1 = 0;
	float pom = 0;
	glColor3f(1, 1, 1);
	glPointSize(10);
	glLineWidth(2.0);
	for (float x = -1; x < 1; x += 2.0 / n)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (float y = -1; y < 1.1; y += 2.0 / n)
		{
			float F = atan(x);
			float B = atan(y * cos(F));
			float koorX = R * cos(B) * sin(F);
			float koorZ = R * cos(B) * cos(F);
			float koorY = R * sin(B);

			float F2 = atan(x+2.0/n);
			float B2 = atan(y * cos(F2));
			float koorX2 = R * cos(B2) * sin(F2);
			float koorZ2 = R * cos(B2) * cos(F2);
			float koorY2 = R * sin(B2);
			glTexCoord2f(s1, x1);
			glVertex3f(koorX, koorY, koorZ);
			x1 += brojDelova;
			/*if (x1 <= 1)
			{
				glTexCoord2f(s1, x1);
				glVertex3f(koorX2, koorY2, koorZ2);
				x1 -= brojDelova;
			}*/
			glTexCoord2f(s1, x1);
			glVertex3f(koorX2, koorY2, koorZ2);
			x1 -= brojDelova;
			s1 += brojDelova;
				
			//x1 += brojDelova;
		}
		glEnd();
		pom += brojDelova;
		s1 =0;
		x1 =pom;
	}
}

void CGLRenderer::DrawEarth(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TSC0);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TSC3);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TSC2);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TSC1);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glRotatef(90, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TSC5);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(180, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TSC4);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);

}

void CGLRenderer::DrawMoon(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, M0);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, M3);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, M2);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, M1);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glRotatef(90, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, M5);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(180, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, M4);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, S0);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, S3);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, S2);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(90, 1, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, S1);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glRotatef(90, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, S5);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
	glRotatef(180, 0, 1, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, S4);
	DrawPatch(R, 20);
	glDisable(GL_TEXTURE_2D);
}
