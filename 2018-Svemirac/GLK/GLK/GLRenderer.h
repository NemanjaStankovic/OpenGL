#pragma once
#include "DImage.h";
class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	UINT LoadTexture(char* fileName);
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,
	void crtajKoordinate();
	void DrawShip();
	void DrawSpaceCube(double a);
	void DrawSpace();
	void DrawTriangle(float d1, float d2, float rep);
	double kameraX, kameraY, kameraZ, udaljenje;
	double alpha, beta;
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 
	UINT back, bot, front, left, right, ShipT1, top;

};
