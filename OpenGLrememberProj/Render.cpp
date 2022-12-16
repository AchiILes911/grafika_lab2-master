#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
const double pi = 3.14159265358979323846;
const int steps = 100;
const float my_angle = pi * 2.0 / steps;

bool textureMode = true;
bool lightMode = true;

struct Point
{
	double x, y, z;
};

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.1, 0.1, 0.1, 1. };
	GLfloat dif[] = { 0.2, 0.2, 0.2, 1. };
	GLfloat spec[] = { 0.3, 0.3, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	double A[] = { -5, 1,  0 };
	double B[] = { -4, 7, 0 };
	double C[] = { 2, 9, 0 };
	double D[] = { 5, 4, 0 };
	double E[] = { 2, 1, 0 };
	double F[] = { 5, -6, 0 };
	double G[] = { 0, -7, 0 };
	double H[] = { 0, 0, 0 };

	double A1[] = { -5, 1,  1 };
	double B1[] = { -4, 7, 1 };
	double C1[] = { 2, 9, 1 };
	double D1[] = { 5, 4, 1 };
	double E1[] = { 2, 1, 1 };
	double F1[] = { 5, -6, 1 };
	double G1[] = { 0, -7, 1 };
	double H1[] = { 0, 0, 1 };

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
		/*glColor3d(1, 0, 0);
		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(E);*/

	glColor3d(0.23, 0.12, 0.67);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(E);

	glColor3d(0.7, 0.77, 0.65);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);

	glColor3d(0.45, 0, 1);
	glVertex3dv(A);
	glVertex3dv(E);
	glVertex3dv(H);

	glColor3d(0, 0.98, 0);
	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(H);

	glColor3d(0.34, 0, 1);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(H);

	glNormal3d(0, 0, 2);
	
	glColor3d(0.544, 0, 0.34);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(E1);

	glColor3d(0, 0.4353, 0.23);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(E1);

	glColor3d(0.7, 0.7, 0);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);

	glColor3d(0.12, 0, 1);
	glVertex3dv(A1);
	glVertex3dv(E1);
	glVertex3dv(H1);

	glColor3d(0, 0, 0.34);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glVertex3dv(H1);

	glColor3d(0.45, 0, 1);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(H1);

	glColor4d(1, 0.34, 0.3, 0.84);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(E);
	glEnd();

	glEnd();


	glBegin(GL_QUADS);

	Point a;
	Point b;
	a.x = B[0] - A[0];
	a.y = B[1] - A[1];
	a.z = B[2] - A[2];

	b.x = A[0] - A1[0];
	b.y = A[1] - A1[1];
	b.z = A[2] - A1[2];

	double n[] = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
	
	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.543, 0, 0);
	glVertex3dv(A);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(B);

	a.x = C[0] - B[0];
	a.y = C[1] - B[1];
	a.z = C[2] - B[2];

	b.x = B[0] - B1[0];
	b.y = B[1] - B1[1];
	b.z = B[2] - B1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0, 0.432, 0.99);
	glVertex3dv(B);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(C);

	a.x = D[0] - C[0];
	a.y = D[1] - C[1];
	a.z = D[2] - C[2];

	b.x = C[0] - C1[0];
	b.y = C[1] - C1[1];
	b.z = C[2] - C1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.4534, 0, 0.53);
	glVertex3dv(C);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(D);
	glColor3d(1, 0, 1);

	a.x = E[0] - D[0];
	a.y = E[1] - D[1];
	a.z = E[2] - D[2];

	b.x = D[0] - D1[0];
	b.y = D[1] - D1[1];
	b.z = D[2] - D1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.4534, 0, 0.53);
	glVertex3dv(D);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glVertex3dv(E);

	a.x = F[0] - E[0];
	a.y = F[1] - E[1];
	a.z = F[2] - E[2];

	b.x = E[0] - E1[0];
	b.y = E[1] - E1[1];
	b.z = E[2] - E1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.4534, 0, 0.53);
	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glVertex3dv(F);

	glColor3d(0.55, 0, 1);

	a.x = G[0] - F[0];
	a.y = G[1] - F[1];
	a.z = G[2] - F[2];

	b.x = F[0] - F1[0];
	b.y = F[1] - F1[1];
	b.z = F[2] - F1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.4534, 0.45, 0.53);
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(G);

	a.x = H[0] - G[0];
	a.y = H[1] - G[1];
	a.z = H[2] - G[2];

	b.x = G[0] - G1[0];
	b.y = G[1] - G1[1];
	b.z = G[2] - G1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.673, 0.12, 0.45);
	glVertex3dv(G);
	glVertex3dv(G1);
	glVertex3dv(H1);
	glVertex3dv(H);

	a.x = A[0] - H[0];
	a.y = A[1] - H[1];
	a.z = A[2] - H[2];

	b.x = H[0] - H1[0];
	b.y = H[1] - H1[1];
	b.z = H[2] - H1[2];

	n[0] = a.y * b.z - a.z * b.y;
	n[1] = a.z * b.x - a.x * b.z;
	n[2] = a.x * b.y - a.y * b.x;

	glNormal3d(n[0], n[1], n[2]);
	glColor3d(0.543, 0.32, 1);
	glVertex3dv(H);
	glVertex3dv(H1);
	glVertex3dv(A1);
	glVertex3dv(A);

	glEnd();
	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;

	ss << "�����. ����: (" << n[0] << ", " << n[1] << ", " << n[2] << ")" << std::endl;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}
