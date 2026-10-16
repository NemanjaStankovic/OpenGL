#pragma once

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	UINT LoadTexture(char* fileName);
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	void crtajLinke();
	void DrawCube(double a);
	void DrawTube(double r1, double r2, double h, int n);
	void DrawCone(double r, double h, int n);
	void DrawPaddle(double length, double width);
	double kameraX, kameraY, kameraZ, udaljenje;
	double alpha, beta, ugaoVetrenjace;
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 
	UINT env, brick;

};
