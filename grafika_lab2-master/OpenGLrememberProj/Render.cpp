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

#define TOP_RIGHT 1.0f,1.0f
#define TOP_LEFT 0.0f,1.0f
#define BOTTOM_RIGHT 1.0f,0.0f
#define BOTTOM_LEFT 0.0f,0.0f

bool textureMode = true;
int ChangetextureMode = 0;
bool lightMode = true;
bool AlphaMode = true;
bool tex = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
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

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
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
	if (key == 'C')
	{
		ChangetextureMode++;
		if (ChangetextureMode == 3)
		{
			ChangetextureMode = 0;
		}
		if (tex)
		{
			if (ChangetextureMode >= 2)
			{
				ChangetextureMode = 0;
			}
		}
	}
	if (key == 'A')
	{
		AlphaMode = !AlphaMode;
	}
	if (key == 'N')
	{
		tex = !tex;
		ChangetextureMode = 0;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId[3];
GLuint newtexId[2];

void UploadTextureInTexId(const char* name, const int NumberOfTexId)
{
	//массив трехбайтных элементов (R G B)
	RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4 4, потомучто выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP(name, &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);


	//генерируем ИД для текстуры
	glGenTextures(1, &texId[NumberOfTexId]);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[NumberOfTexId]);

	//загружаем текстуру в видеопямять, в оперативке нам больше она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void UploadTextureInNewTexId(const char* name, const int NumberOfTexId)
{
	//массив трехбайтных элементов (R G B)
	RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4 4, потомучто выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP(name, &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);


	//генерируем ИД для текстуры
	glGenTextures(1, &newtexId[NumberOfTexId]);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, newtexId[NumberOfTexId]);

	//загружаем текстуру в видеопямять, в оперативке нам больше она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	UploadTextureInTexId("texture1.bmp", 0);
	UploadTextureInTexId("texture2.bmp", 1);
	UploadTextureInTexId("texture3.bmp", 2);
	UploadTextureInNewTexId("texture.bmp", 0);
	UploadTextureInNewTexId("Sectexture.bmp", 1);

	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}
void NormalizeVector(double* vec)
{
	double modVector = -sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));

	for (int i = 0; i < 3; ++i)
	{
		vec[i] /= modVector;
	}

}

double* FindNormal(double x, double y, double z, double x1, double y1, double z1, double x2, double y2, double z2, int FlagSwap = 0) // A - "общая"
{
	double vectorA[3], vectorB[3];
	double a[3] = { x,y,z }, b[3] = { x1,y1,z1 }, c[3] = { x2,y2,z2 };

	for (int i = 0; i < 3; ++i) // Получаем вектор A и B
	{
		vectorA[i] = a[i] - c[i];
		vectorB[i] = b[i] - c[i];
	}

	double VectorNormal[3];

	//VectorNormal[0] = a[1] * b[2] - a[2] * b[1];
	//VectorNormal[1] = a[2] * b[0] - a[0] * b[2];
	//VectorNormal[2] = a[0] * b[1] - a[1] * b[0];

	VectorNormal[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
	VectorNormal[1] = -vectorA[0] * vectorB[2] + vectorB[0] * vectorA[2];
	VectorNormal[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];

	NormalizeVector(VectorNormal);

	if (FlagSwap != 0)
	{
		for (int i = 0; i < 3; ++i) // Получаем вектор A и B
		{
			VectorNormal[i] *= -1;
		}
	}

	return VectorNormal;
}

double* FindNormal(double* a, double* b, double* c, int FlagSwap = 0) // A - "общая"
{
	double vectorA[3], vectorB[3];

	for (int i = 0; i < 3; ++i) // Получаем вектор A и B
	{
		vectorA[i] = a[i] - c[i];
		vectorB[i] = b[i] - c[i];
	}

	double VectorNormal[3];

	//VectorNormal[0] = a[1] * b[2] - a[2] * b[1];
	//VectorNormal[1] = a[2] * b[0] - a[0] * b[2];
	//VectorNormal[2] = a[0] * b[1] - a[1] * b[0];

	VectorNormal[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
	VectorNormal[1] = -vectorA[0] * vectorB[2] + vectorB[0] * vectorA[2];
	VectorNormal[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];

	NormalizeVector(VectorNormal);

	if (FlagSwap != 0)
	{
		for (int i = 0; i < 3; ++i) // Получаем вектор A и B
		{
			VectorNormal[i] *= -1;
		}
	}

	return VectorNormal;
}

double NewCoordY(double oldCoord)
{
	double NewCoord = (oldCoord) / -15; // оставить или убрать +3?
	return NewCoord;
}

double NewCoordX(double oldCoord)
{
	double NewCoord = oldCoord / -12;
	return NewCoord;
}

void Circles()
{
	double a3[] = { -8, -2, 0 };
	double b3[] = { -11, -7, 0 };
	double x01 = -9.5, y01 = -4.5, r = 2.9;
	std::vector <double> coor_x1;
	std::vector <double> coor_y1;

	coor_x1.push_back(b3[0]);
	coor_y1.push_back(b3[1]);
	for (double i = -149; i < 180 - 149; i += 0.09)
	{
		double y, x;
		coor_x1.push_back(x01 + r * sin(i * PI / 180));
		coor_y1.push_back(y01 + r * cos(i * PI / 180));
	}
	coor_x1.push_back(a3[0]);
	coor_y1.push_back(a3[1]);
	//выпуклая стенка
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	for (double i = 0; i < coor_x1.size() - 1; i++)
	{
		glNormal3dv(FindNormal(coor_x1[i], coor_y1[i], 6, coor_x1[i], coor_y1[i], 2, coor_x1[i + 1], coor_y1[i + 1], 2));
		glTexCoord2d(i / (coor_x1.size() - 1), 1);
		glVertex3d(coor_x1[i], coor_y1[i], 4);
		glTexCoord2d(i / (coor_x1.size() - 1), 0);
		glVertex3d(coor_x1[i], coor_y1[i], 0);
		glTexCoord2d(i / (coor_x1.size() - 1), 0);
		glVertex3d(coor_x1[i + 1], coor_y1[i + 1], 0);
		glTexCoord2d(i / (coor_x1.size() - 1), 1);
		glVertex3d(coor_x1[i + 1], coor_y1[i + 1], 4);
	}
	glEnd();
	//Выпуклая днище
	glBegin(GL_POLYGON);
	glColor3d(0.7, 0.7, 0.7);
	glNormal3dv(FindNormal(coor_x1[0], coor_y1[0], 2, coor_x1[1], coor_y1[1], 2, coor_x1[1 + 1], coor_y1[1 + 1], 2));
	for (int i = 0; i < coor_x1.size() - 1; i++)
	{
		glTexCoord2d(NewCoordX(coor_x1[i]), NewCoordY(coor_y1[i]));
		glVertex3d(coor_x1[i], coor_y1[i], 0);
		glTexCoord2d(NewCoordX(coor_x1[i + 1]), NewCoordY(coor_y1[i + 1]));
		glVertex3d(coor_x1[i + 1], coor_y1[i + 1], 0);
	}
	glEnd();
	//Впуклая часть СТЕНКА
	double a2[] = { -2, -8, 4 };
	double b2[] = { -8, -15, 4 };
	double x0 = -1.5, y0 = -14.5;
	r = 6.52;
	std::vector <double> coor_x;
	std::vector <double> coor_y;
	coor_x.push_back(a2[0]);
	coor_y.push_back(a2[1]);
	double povorot = -(atan(a2[0] - b2[0] / a2[1] - b2[1]) * 180 / PI);
	double alpha_div2 = 41;
	for (double i = 51 - alpha_div2; i <= 58.1 + alpha_div2; i += 0.09)
	{
		double y1, x1;
		x1 = r * cos(i * PI / 180); // Строим эллипс в начале координат
		y1 = r * sin(i * PI / 180);
		coor_x.push_back(x1 * cos(povorot * PI / 180) + y1 * sin(povorot * PI / 180) + x0); // Формула для поворота эллипса относительно начала координат и перенос цента эллипса в точку M
		coor_y.push_back(-x1 * sin(povorot * PI / 180) + y1 * cos(povorot * PI / 180) + y0);

		/*double y, x;
		coor_x.push_back(x0 + r * sin(i * PI / 180));
		coor_y.push_back(y0 + r * cos(i * PI / 180));*/
	}
	coor_x.push_back(b2[0]);
	coor_y.push_back(b2[1]);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	for (double i = 0; i < coor_x.size() - 1; i++)
	{
		glNormal3dv(FindNormal(coor_x[i], coor_y[i], 6, coor_x[i], coor_y[i], 2, coor_x[i + 1], coor_y[i + 1], 2));
		glTexCoord2d(i / (coor_x.size() - 1), 1); 
		glVertex3d(coor_x[i], coor_y[i], 4);
		glTexCoord2d(i / (coor_x.size() - 1), 0); 
		glVertex3d(coor_x[i], coor_y[i], 0);
		glTexCoord2d(i / (coor_x.size() - 1), 0); 
		glVertex3d(coor_x[i + 1], coor_y[i + 1], 0);
		glTexCoord2d(i / (coor_x.size() - 1), 1); 
		glVertex3d(coor_x[i + 1], coor_y[i + 1], 4);
	}
	glEnd();

	//Впуклость ДНИЩЕ

	double b[] = { -4, -4, 4 };
	double a[] = { -12, -12, 4 };
	double b1[] = { -4, -4, 0 };
	double a1[] = { -12, -12, 0 };
	glColor3d(0.7, 0.7, 0.7);
	glBegin(GL_TRIANGLES);
	glNormal3dv(FindNormal(coor_x[0], coor_y[0], 2, coor_x[1], coor_y[1], 2, coor_x[1 + 1], coor_y[1 + 1], 2));
	for (int i = 0; i < (coor_x.size() - 1) / 2; i++)
	{
		glTexCoord2d(NewCoordX(b[0]), NewCoordY(b[1])); 
		glVertex3dv(b1);
		glTexCoord2d(NewCoordX(coor_x[i]), NewCoordY(coor_y[i])); 
		glVertex3d(coor_x[i], coor_y[i], 0);
		glTexCoord2d(NewCoordX(coor_x[i + 1]), NewCoordY(coor_y[i + 1])); 
		glVertex3d(coor_x[i + 1], coor_y[i + 1], 0);
	}
	for (int i = (coor_x.size() - 1) / 2; i < coor_x.size() - 1; i++)
	{
		glTexCoord2d(NewCoordX(a[0]), NewCoordY(a[1])); 
		glVertex3dv(a1);
		glTexCoord2d(NewCoordX(coor_x[i]), NewCoordY(coor_y[i])); 
		glVertex3d(coor_x[i], coor_y[i], 0);
		glTexCoord2d(NewCoordX(coor_x[i + 1]), NewCoordY(coor_y[i + 1])); 
		glVertex3d(coor_x[i + 1], coor_y[i + 1], 0);
	}
	glTexCoord2d(NewCoordX(a[0]), NewCoordY(a[1])); 
	glVertex3dv(a1);
	glTexCoord2d(NewCoordX(b[0]), NewCoordY(b[1])); 
	glVertex3dv(b1);
	glTexCoord2d(NewCoordX(coor_x[(coor_x.size() - 1) / 2]), NewCoordY(coor_y[(coor_y.size() - 1) / 2])); 
	glVertex3d(coor_x[(coor_x.size() - 1) / 2], coor_y[(coor_y.size() - 1) / 2], 0);
	glEnd();

	//Впуклость КРЫШКА
		//альфаналожение
	if (AlphaMode == true)
	{
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glColor4d(0.8, 0.2, 0.8, 0.5);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}
	
	glBegin(GL_TRIANGLES);
	glNormal3dv(FindNormal(coor_x[0], coor_y[0], 2, coor_x[1], coor_y[1], 2, coor_x[1 + 1], coor_y[1 + 1], 2, 1));
	for (int i = 0; i < (coor_x.size() - 1) / 2; i++)
	{
		glTexCoord2d(NewCoordX(b[0]), NewCoordY(b[1])); 
		glVertex3dv(b);
		glTexCoord2d(NewCoordX(coor_x[i]), NewCoordY(coor_y[i])); 
		glVertex3d(coor_x[i], coor_y[i], 4);
		glTexCoord2d(NewCoordX(coor_x[i + 1]), NewCoordY(coor_y[i + 1])); 
		glVertex3d(coor_x[i + 1], coor_y[i + 1], 4);
	}
	for (int i = (coor_x.size() - 1) / 2; i < coor_x.size() - 1; i++)
	{
		glTexCoord2d(NewCoordX(a[0]), NewCoordY(a[1])); 
		glVertex3dv(a);
		glTexCoord2d(NewCoordX(coor_x[i]), NewCoordY(coor_y[i])); 
		glVertex3d(coor_x[i], coor_y[i], 4);
		glTexCoord2d(NewCoordX(coor_x[i + 1]), NewCoordY(coor_y[i + 1])); 
		glVertex3d(coor_x[i + 1], coor_y[i + 1], 4);
	}
	glTexCoord2d(NewCoordX(a[0]), NewCoordY(a[1])); 
	glVertex3dv(a);
	glTexCoord2d(NewCoordX(b[0]), NewCoordY(b[1])); 
	glVertex3dv(b);
	glTexCoord2d(NewCoordX(coor_x[(coor_x.size() - 1) / 2]), NewCoordY(coor_y[(coor_y.size() - 1) / 2])); 
	glVertex3d(coor_x[(coor_x.size() - 1) / 2], coor_y[(coor_y.size() - 1) / 2], 4);
	glEnd();
	//Выпуклая крыша
	glBegin(GL_POLYGON);
	glNormal3dv(FindNormal(coor_x1[0], coor_y1[0], 6, coor_x1[1], coor_y1[1], 6, coor_x1[1 + 1], coor_y1[1 + 1], 6, 1));
	for (int i = 0; i < coor_x1.size() - 1; i++)
	{
		glTexCoord2d(NewCoordX(coor_x1[i]), NewCoordY(coor_y1[i]));
		glVertex3d(coor_x1[i], coor_y1[i], 4);
		glTexCoord2d(NewCoordX(coor_x1[i + 1]), NewCoordY(coor_y1[i + 1]));
		glVertex3d(coor_x1[i + 1], coor_y1[i + 1], 4);
	}
	glEnd();
}
void bottom()
{
	double A[] = { 0, 0, 0 };
	double B[] = { -8, -2, 0 };
	double C[] = { -11, -7, 0 };

	double A2[] = { -11, -7, 0 };
	double B2[] = { -4, -4, 0 };
	double C2[] = { 0, 0, 0 };

	double A5[] = { -2, -8, 0 };
	double B5[] = { -4, -4, 0 };
	double C5[] = { 0, 0, 0 };

	glBegin(GL_TRIANGLES);
	glColor3d(0.7, 0.7, 0.7);
	glNormal3dv(FindNormal(A, B, C));
	glTexCoord2d(NewCoordX(A[0]), NewCoordY(A[1]));
	glVertex3dv(A);
	glTexCoord2d(NewCoordX(B[0]), NewCoordY(B[1]));
	glVertex3dv(B);
	glTexCoord2d(NewCoordX(C[0]), NewCoordY(C[1]));
	glVertex3dv(C);
	glNormal3dv(FindNormal(A2, B2, C2));
	glTexCoord2d(NewCoordX(A2[0]), NewCoordY(A2[1]));
	glVertex3dv(A2);
	glTexCoord2d(NewCoordX(B2[0]), NewCoordY(B2[1]));
	glVertex3dv(B2);
	glTexCoord2d(NewCoordX(C2[0]), NewCoordY(C2[1]));
	glVertex3dv(C2);
	glNormal3dv(FindNormal(A5, B5, C5, 1));
	glTexCoord2d(NewCoordX(A5[0]), NewCoordY(A5[1]));
	glVertex3dv(A5);
	glTexCoord2d(NewCoordX(B5[0]), NewCoordY(B5[1]));
	glVertex3dv(B5);
	glTexCoord2d(NewCoordX(C5[0]), NewCoordY(C5[1]));
	glVertex3dv(C5);
	glEnd();
}

void roof()
{
	double A[] = { 0, 0, 4 };
	double B[] = { -8, -2, 4 };
	double C[] = { -11, -7, 4 };

	double A2[] = { -11, -7, 4 };
	double B2[] = { -4, -4, 4 };
	double C2[] = { 0, 0, 4 };

	double A5[] = { -2, -8, 4 };
	double B5[] = { -4, -4, 4 };
	double C5[] = { 0, 0, 4 };

	glBegin(GL_TRIANGLES);
	glNormal3dv(FindNormal(A, B, C, 1));
	glTexCoord2d(NewCoordX(A[0]), NewCoordY(A[1]));
	glVertex3dv(A);
	glTexCoord2d(NewCoordX(B[0]), NewCoordY(B[1]));
	glVertex3dv(B);
	glTexCoord2d(NewCoordX(C[0]), NewCoordY(C[1]));
	glVertex3dv(C);
	glNormal3dv(FindNormal(A2, B2, C2, 1));
	glTexCoord2d(NewCoordX(A2[0]), NewCoordY(A2[1]));
	glVertex3dv(A2);
	glTexCoord2d(NewCoordX(B2[0]), NewCoordY(B2[1]));
	glVertex3dv(B2);
	glTexCoord2d(NewCoordX(C2[0]), NewCoordY(C2[1]));
	glVertex3dv(C2);
	glNormal3dv(FindNormal(A5, B5, C5));
	glTexCoord2d(NewCoordX(A5[0]), NewCoordY(A5[1]));
	glVertex3dv(A5);
	glTexCoord2d(NewCoordX(B5[0]), NewCoordY(B5[1]));
	glVertex3dv(B5);
	glTexCoord2d(NewCoordX(C5[0]), NewCoordY(C5[1]));
	glVertex3dv(C5);
	glEnd();
}

void walls()
{
	double A[] = { 0, 0, 0 };
	double B[] = { 0, 0, 4 };
	double C[] = { -8, -2, 4 };
	double D[] = { -8, -2, 0 };

	double A3[] = { -11, -7, 0 };
	double B3[] = { -11, -7, 4 };
	double C3[] = { -4, -4, 4 };
	double D3[] = { -4, -4, 0 };

	double A4[] = { -4, -4, 0 };
	double B4[] = { -4, -4, 4 };
	double C4[] = { -12, -12, 4 };
	double D4[] = { -12, -12, 0 };

	double A5[] = { -12, -12, 0 };
	double B5[] = { -12, -12, 4 };
	double C5[] = { -8, -15, 4 };
	double D5[] = { -8, -15, 0 };

	double A7[] = { -2, -8, 0 };
	double B7[] = { -2, -8, 4 };
	double C7[] = { 0, 0, 4 };
	double D7[] = { 0, 0, 0 };

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glNormal3dv(FindNormal(A, B, C));
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(A);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(B);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(C);
	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(D);
	glNormal3dv(FindNormal(A3, B3, C3));
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(A3);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(B3);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(C3);
	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(D3);
	glNormal3dv(FindNormal(A4, B4, C4));
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(A4);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(B4);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(C4);
	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(D4);
	glNormal3dv(FindNormal(A5, B5, C5));
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(A5);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(B5);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(C5);
	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(D5);
	glNormal3dv(FindNormal(A7, B7, C7));
	glTexCoord2d(BOTTOM_LEFT);
	glVertex3dv(A7);
	glTexCoord2d(TOP_LEFT);
	glVertex3dv(B7);
	glTexCoord2d(TOP_RIGHT);
	glVertex3dv(C7);
	glTexCoord2d(BOTTOM_RIGHT);
	glVertex3dv(D7);
	glEnd();
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (tex)
	{
		glBindTexture(GL_TEXTURE_2D, newtexId[ChangetextureMode]);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, texId[ChangetextureMode]);
	}
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);




	//настройка материала
	GLfloat amb[] = { 0.4, 0.4, 0.2, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	double rot[] = { 1, 0, 2 };
	double tran[] = { 0, 0, 3 };
	glPushMatrix();
	//glRotated(90, rot[0], rot[1], rot[2]);
	//glTranslated(tran[0], tran[1], tran[2]);
	bottom();
	walls();
	Circles();
	roof();
	/*pyramid();*/
	glPopMatrix();

	//Начало рисования квадратика станкина
	/*double A[2] = { -4, 0 };
	double B[2] = { 8, 0 };
	double C[2] = { 8, 8 };
	double D[2] = { -4, 8 };

	

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 200);
	rec.setPosition(10, ogl->getHeight() - 200 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "C - Поменять текстуру" << std::endl;
	ss << "A - Включить Альфаналожение" << std::endl;\
	ss << "N - Поменять пак текстур" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}