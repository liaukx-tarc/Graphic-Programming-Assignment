#include <Windows.h>
#include <Windowsx.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#include <string>
using namespace std;

#pragma comment (lib, "GLU32.lib")
#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"

//Texture
//step 1: Variable declaration
BITMAP BMP;			 //bitmap structure
HBITMAP hBMP = NULL;//bitmap handle 
string str;
GLuint textures;
int textureType = 0;
//LIGHTING
bool isLightOn = false;

GLfloat amb[] = { 0.0f, 0.0f, 0.0f };
GLfloat posA[] = { 0.0f, 0.0f, 0.0f };

GLfloat dif[] = { 1.0f, 1.0f, 1.0f };
GLfloat posD[] = { 0.0f, 0.0f, 0.0f };

void drawCone(double br, double h);
void drawConeShape(double br, double h);
void drawScope(double r);
//ARM
float initialArmRotate = 0.0f;
float initialArmRotate1 = 0.0f;
float armRotate = 0.0f;
float armRotate1 = 0.0f;
float initialArmMove = 0.0f;
float initialArmMove1 = 0.0f;
float armMove = 0.0f;
float armMove1 = 0.0f;
float initialFingerMove = 0.0f;
float fingerMove = 0.0f;
float initialThumbMove = 0.0f;
float thumbMove = 0.0f;
float bodyAngle = 0.0f;
void drawRec(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, GLenum type);
void drawLeftHand();
void drawRightHand();
void adjustFingerMove(float tx, float ty, float tz, float rx, float ry, float rz, float maxA, float minA);

float speed = 1.5f;
float initialLeftArmSpeed = 0.0f, leftArmSpeed = 0.0f, leftArmMaxAngle = 0.0f, leftArmMinAngle = 0.0f;
float initialRightArmSpeed = 0.0f, rightArmSpeed = 0.0f, rightArmMaxAngle = 0.0f, rightArmMinAngle = 0.0f;
void shieldPentagon(float minZ, float maxZ, GLenum type1, GLenum type2);
void DrawShield();
void DrawHandle();
void controlShield();
bool isShield = false;

//camera & model rotate
float bodyRotateY;
float bodyRotateX;
float lastX = 0.0f, lastY = 0.0f;
float zoom = 0;
bool switchCamera = true;
float oNear = 0.0, oFar = 10.0;
float pZMin = 1.0, pZMax = 17.0;
float pTx = 0, pTy = 0;

//buttonsetting
bool isMovingArm = false;

GLuint armorTexture[8];
//Leg
GLUquadricObj* var = NULL;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
		switch (wParam) {
		case MK_LBUTTON:
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			bodyRotateY -= xPos - lastX;
			bodyRotateX -= yPos - lastY;
			lastX = xPos;
			lastY = yPos;
			break;
		}
	case WM_LBUTTONDOWN:
		lastX = GET_X_LPARAM(lParam);
		lastY = GET_Y_LPARAM(lParam);
		break;
	case WM_MOUSEWHEEL:
		zoom += GET_WHEEL_DELTA_WPARAM(wParam) / 300.0f;
		break;

	case WM_KEYDOWN:
		if (wParam == VK_SPACE) {
			initialArmRotate = 0.0f;
			initialArmRotate1 = 0.0f;
			armRotate = 0.0f;
			armRotate1 = 0.0f;
			initialArmMove = 0.0f;
			initialArmMove1 = 0.0f;
			armMove = 0.0f;
			armMove1 = 0.0f;
			initialFingerMove = 0.0f;
			fingerMove = 0.0f;
			initialThumbMove = 0.0f;
			thumbMove = 0.0f;
			leftArmMaxAngle = 0.0f;
			rightArmMaxAngle = 0.0f;
		}
		else if (wParam == 'H')
		{
			isMovingArm = !isMovingArm;
		}
		else if (wParam == 'A') {
			//AntiClockwise - L ARM
			if (isMovingArm)
				armRotate = 0.025f;
			else
				pTx += 0.1;
		}
		else if (wParam == 'D') {
			//Clockwise - L ARM
			if (isMovingArm)
				armRotate = -0.025f;
			else
				pTx -= 0.1;
		}
		else if (wParam == 'W') {
			//AntiClockwise - L ARM
			if (isMovingArm)
				armMove = 0.1f;
			else
			{
				pTy -= 0.1;
			}
		}
		else if (wParam == 'S') {
			//Clockwise - L ARM
			if (isMovingArm)
				armMove = -0.1f;
			else if (pTy < 1)
			{
				pTy += 0.1;
			}
		}
		else if (wParam == VK_UP) {
			//AntiClockwise - R ARM
			if (isMovingArm)
				armMove1 = 0.1f;
		}
		else if (wParam == VK_DOWN) {
			//Clockwise - R ARM
			if (isMovingArm)
				armMove1 = -0.1f;
		}
		else if (wParam == VK_LEFT) {
			//AntiClockwise - R ARM
			if (isMovingArm)
				armRotate1 = -0.025f;
		}
		else if (wParam == VK_RIGHT) {
			//Clockwise - R ARM
			if (isMovingArm)
				armRotate1 = 0.025f;
		}
		//'Q' - Stop arm move and rotate
		else if (wParam == 'Q') {
			if (isMovingArm)
			{
				armMove = 0.0f;
				armRotate = 0.0f;
				armRotate1 = 0.0f;
				fingerMove = 0.0f;
				thumbMove = 0.0f;
			}
		}
		//'C' - close hand
		else if (wParam == 'C') {
			if (isMovingArm)
			{
				fingerMove = 0.005f;
				thumbMove = 0.02f;
			}
			else
			{
				switchCamera = !switchCamera;
				pTx = 0, pTy = 0;
			}
		}
		//'O' - open hand
		else if (wParam == 'O') {
			if (isMovingArm)
			{
				fingerMove = -0.005f;
				thumbMove = -0.02f;
			}
		}
		else if (wParam == 'K') {// to left lower arm left right
			if (isMovingArm)
			{
				leftArmMaxAngle = 80.0f;

				if (initialLeftArmSpeed == 0.0f) {

					leftArmSpeed = speed;

				}
				if (initialLeftArmSpeed == leftArmMaxAngle) {

					leftArmSpeed = -speed;

				}

				rightArmSpeed = -speed;
			}

		}
		else if (wParam == 'P') {// to right lower arm left right
			if (isMovingArm)
			{
				rightArmMaxAngle = 80.0f;

				if (initialRightArmSpeed == 0.0f) {

					rightArmSpeed = speed;

				}
				if (initialRightArmSpeed == rightArmMaxAngle) {

					rightArmSpeed = -speed;

				}
				leftArmSpeed = -speed;

			}
		}
		else if (wParam == 'T') {
			textureType++;
			if (textureType >= 8)
			{
				textureType = 0;
			}
		}//Lighting
		else if (wParam == '1') {
			isLightOn = !isLightOn;
		}
		if (isLightOn) {

			//Move light up 
			if (wParam == VK_NUMPAD8) {
				if (posA[1] <= 1.0f && posD[1] <= 1.0f) {
					posD[1] += 0.1f;
				}
			}
			//Move light down
			if (wParam == VK_NUMPAD2) {
				if (posA[1] >= -1.0f && posD[1] >= -1.0f) {
					posD[1] -= 0.1f;
				}
			}
			//Move light left 
			if (wParam == VK_NUMPAD4) {
				if (posA[0] >= -1.0f && posD[0] >= -1.0f) {
					posD[0] -= 0.1f;
				}
			}
			//Move light right
			if (wParam == VK_NUMPAD6) {
				if (posA[0] <= 1.0f && posD[0] <= 1.0f) {
					posD[0] += 0.1f;
				}
			}
			//Move light near
			if (wParam == VK_NUMPAD7) {
				if (posA[2] >= -1.0f && posD[2] >= -1.0f) {
					posD[2] -= 0.1f;
				}
			}
			//Move light far 
			if (wParam == VK_NUMPAD9) {
				if (posA[2] <= 1.0f && posD[2] <= 1.0f) {
					posD[2] += 0.1f;
				}
			}

		}

		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

GLuint loadTexture(LPCSTR filename) {
	//take from step 1
	GLuint texture = 0;  //texture name

	//Step3: Initialize texture info
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);//bitmap handle 
	GetObject(hBMP, sizeof(BMP), &BMP);

	//Step 4:Assign texture to polygon.
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	//take from step 5
	DeleteObject(hBMP);
	return texture;
}

//color
void color(int r, int g, int b)
{
	glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
}

//SHAPE
void drawSphere(double r, GLenum type) {
	GLUquadricObj* sphere = NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, type);
	gluSphere(sphere, r, 30, 30);
	gluDeleteQuadric(sphere);
}

void drawCircle(float x, float y, float r, GLenum type) {
	glBegin(type);
	glVertex2f(x, y);
	for (float angle = 0; angle <= 360; angle += 0.1) {
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void drawCylinder(double tr, double br, double h, GLenum type) {
	GLUquadricObj* cylinder = NULL;
	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, type);
	gluCylinder(cylinder, tr, br, h, 30, 30);
	gluDeleteQuadric(cylinder);
}

void drawPyramid(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float divX, float divZ, GLenum type, GLenum base) {

	//face - back
	glBegin(type);
	glVertex3f(minX, minY, minZ);
	glVertex3f(maxX, minY, minZ);
	glVertex3f(maxX / divX, maxY, maxZ / divZ);
	glEnd();

	//face - top
	glBegin(type);
	glVertex3f(minX, minY, maxZ);
	glVertex3f(minX, minY, minZ);
	glVertex3f(maxX / divX, maxY, maxZ / divZ);
	glEnd();

	//face - bottom
	glBegin(type);
	glVertex3f(maxX, minY, minZ);
	glVertex3f(maxX, minY, maxZ);
	glVertex3f(maxX / divX, maxY, maxZ / divZ);
	glEnd();

	//face - front
	glBegin(type);
	glVertex3f(minX, minY, maxZ);
	glVertex3f(maxX, minY, maxZ);
	glVertex3f(maxX / divX, maxY, maxZ / divZ);
	glEnd();

	//face - base
	glBegin(base);
	glVertex3f(minX, minY, maxZ);
	glVertex3f(maxX, minY, maxZ);
	glVertex3f(maxX, minY, minZ);
	glVertex3f(minX, minY, minZ);
	glEnd();

}

void drawRec(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, GLenum type) {
	//Back
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, minZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, minY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX, minY, minZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, maxY, minZ);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, minY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, minY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX, minY, minZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, minY, maxZ);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, maxY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(minX, minY, minZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(minX, minY, maxZ);
	glEnd();

	//Top
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, maxY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX, maxY, minZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, maxY, maxZ);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(maxX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(maxX, maxY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX, minY, minZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, minY, maxZ);
	glEnd();

	//Front
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, minY, maxZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX, minY, maxZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, maxY, maxZ);
	glEnd();

}

void drawQuad(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float diffLength, GLenum type) {
	//Back
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, minZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX - diffLength, minY, minZ - diffLength);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX + diffLength, minY, minZ - diffLength);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, maxY, minZ);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX - diffLength, minY, maxZ + diffLength);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX - diffLength, minY, minZ - diffLength);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX + diffLength, minY, minZ - diffLength);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX + diffLength, minY, maxZ + diffLength);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, maxY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(minX - diffLength, minY, minZ - diffLength);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(minX - diffLength, minY, maxZ + diffLength);
	glEnd();

	//Top
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX, maxY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX, maxY, minZ);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, maxY, maxZ);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(maxX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(maxX, maxY, minZ);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX + diffLength, minY, minZ - diffLength);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX + diffLength, minY, maxZ + diffLength);
	glEnd();

	//Front
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(minX, maxY, maxZ);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(minX - diffLength, minY, maxZ + diffLength);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(maxX + diffLength, minY, maxZ + diffLength);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(maxX, maxY, maxZ);
	glEnd();

}

void triangle(GLenum type)
{
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 1, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1, 0, 0);
	glEnd();
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, 1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 1, 1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1, 0, 1);
	glEnd();
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0, 1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 1, 1);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 1, 0);
	glEnd();
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 1, 1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 1, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1, 0, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-1, 0, 1);
	glEnd();
	glBegin(type);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1, 0, 1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1, 0, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0, 1);
	glEnd();
}

void cylinder(float x, float y, float z, float topSize)
{
	GLUquadricObj* cylinder = NULL;
	cylinder = gluNewQuadric();
	float theta = 0;
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glScaled(x, z, y);
	glPushMatrix();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, topSize, 1, 1, 30, 30);
	glPushMatrix();
	glTranslatef(0, 0, 1);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++) {
		theta = i * 3.1416 / 180;
		glVertex2f(0 + 1 * cos(theta), 0 + 1 * sin(theta));
	}
	glEnd();
	glPopMatrix();
	glPushMatrix();
	if (topSize != 0)
	{
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			theta = i * 3.1416 / 180;
			glVertex2f(0 + topSize * cos(theta), 0 + topSize * sin(theta));
		}
		glEnd();
	}
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	gluDeleteQuadric(cylinder);
}

void leftHand() {
	//Palm
	drawRec(1.5f, 2.1f, -0.05f, 0.55f, 0.15f, 0.3f, GL_QUADS);
	glPushMatrix();
	//Thumb
	drawPyramid(1.5, 1.7f, 0.55f, 0.65f, 0.2f, 0.3f, 1.08f, 2.0f, GL_TRIANGLES, GL_QUADS);

	glPushMatrix();
	//initial position of the thumb finger(inner, outer) - no animation
	glTranslatef(1.56f, 0.70f, 0.3f);
	glRotatef(42.0f, 0.0f, 0.0f, 0.5f);
	glTranslatef(-1.56f, -0.70f, -0.3f);

	//inner
	glTranslatef(1.56f, 0.65f, 0.3f);
	glRotatef(initialThumbMove, 0.0f, -0.5f, 0.0f);
	initialThumbMove += thumbMove;
	glTranslatef(-1.56f, -0.65f, -0.3f);

	if (initialThumbMove >= 45.0f) {

		initialThumbMove = 45.0f;
		thumbMove = 0.0f;

	}
	else if (initialThumbMove <= 0.0f) {

		initialThumbMove = 0.0f;
		thumbMove = 0.0f;

	}
	drawRec(1.56f, 1.76f, 0.50f, 0.65f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(1.76f, 0.65f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(1.76f, 1.96, 0.50f, 0.65f, 0.2f, 0.3f, GL_QUADS);
	glPopMatrix();
	glPopMatrix();

	//Index Finger
	glPushMatrix();
	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, 0.40f, 0.52f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, 0.40f, 0.52f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.50f, 0.40f, 0.52f, 0.2f, 0.3f, GL_QUADS);

	glPopMatrix();

	//Middle Finger
	glPushMatrix();
	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, 0.25f, 0.37f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, 0.25f, 0.37f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.6f, 0.25f, 0.37f, 0.2f, 0.3f, GL_QUADS);
	glPopMatrix();

	//Ring Finger
	glPushMatrix();
	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, 0.10f, 0.22f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, 0.10f, 0.22f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.55f, 0.10f, 0.22f, 0.2f, 0.3f, GL_QUADS);
	glPopMatrix();

	//Little Finger
	glPushMatrix();
	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, -0.05f, 0.07f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, -0.05f, 0.07f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.45f, -0.05f, 0.07f, 0.2f, 0.3f, GL_QUADS);

	glPopMatrix();
}

void rightHand() {
	//Palm
	drawRec(1.5f, 2.1f, -0.05f, 0.55f, 0.15f, 0.3f, GL_QUADS);
	glPushMatrix();
	glTranslatef(0, -0.8, 0);
	glRotated(-90, 0, 0, 1);
	glTranslatef(-2.35, 1, 0);

	//Thumb
	drawPyramid(1.5, 1.7f, 0.55f, 0.65f, 0.2f, 0.3f, 1.08f, 2.0f, GL_TRIANGLES, GL_QUADS);

	glPushMatrix();
	glTranslatef(1.56f, 0.70f, 0.3f);
	glRotatef(42.0f, 0.0f, 0.0f, 0.5f);
	glTranslatef(-1.56f, -0.70f, -0.3f);

	//inner
	glTranslatef(1.56f, 0.65f, 0.3f);
	glRotatef(initialThumbMove, 0.0f, -0.5f, 0.0f);
	initialThumbMove += thumbMove;
	glTranslatef(-1.56f, -0.65f, -0.3f);

	if (initialThumbMove >= 45.0f) {

		initialThumbMove = 45.0f;
		thumbMove = 0.0f;

	}
	else if (initialThumbMove <= 0.0f) {

		initialThumbMove = 0.0f;
		thumbMove = 0.0f;

	}
	drawRec(1.56f, 1.76f, 0.50f, 0.65f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(1.76f, 0.65f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(1.76f, 1.96, 0.50f, 0.65f, 0.2f, 0.3f, GL_QUADS);
	glPopMatrix();
	glPopMatrix();

	//Index Finger
	glPushMatrix();

	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, 0.40f, 0.52f, 0.2f, 0.3f, GL_QUADS);
	glColor3f(1, 1, 1);

	//middle
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, 0.40f, 0.52f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.5f, 0.40f, 0.52f, 0.2f, 0.3f, GL_QUADS);

	glPopMatrix();

	//Middle Finger
	glPushMatrix();

	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, 0.25f, 0.37f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, 0.25f, 0.37f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.6f, 0.25f, 0.37f, 0.2f, 0.3f, GL_QUADS);
	glPopMatrix();

	//Ring Finger
	glPushMatrix();
	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, 0.10f, 0.22f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, 0.10f, 0.22f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.55f, 0.10f, 0.22f, 0.2f, 0.3f, GL_QUADS);
	glPopMatrix();

	//Little Finger
	glPushMatrix();

	//inner
	adjustFingerMove(2.10f, 0.50f, 0.3f, 0.0f, 0.5f, 0.0f, 60.0f, 0.0f);
	drawRec(2.00f, 2.17f, -0.05f, 0.07f, 0.2f, 0.3f, GL_QUADS);

	//middle
	adjustFingerMove(2.10f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.10f, 2.32f, -0.05f, 0.07f, 0.2f, 0.3f, GL_QUADS);

	//outer
	adjustFingerMove(2.25f, 0.20f, 0.3f, 0.0f, 0.5f, 0.0f, 90.0f, 0.0f);
	drawRec(2.25f, 2.45f, -0.05f, 0.07f, 0.2f, 0.3f, GL_QUADS);

	glPopMatrix();
}

void adjustFingerMove(float tx, float ty, float tz, float rx, float ry, float rz, float maxA, float minA) {
	glTranslatef(tx, ty, tz);
	glRotatef(initialFingerMove, rx, -ry, rz);
	initialFingerMove += fingerMove;
	glTranslatef(-tx, -ty, -tz);

	if (initialFingerMove >= maxA) {

		initialFingerMove = maxA;
		fingerMove = 0.0f;

	}
	else if (initialFingerMove <= minA) {

		initialFingerMove = minA;
		fingerMove = minA;

	}
}

void drawLeftHand() {
	glPushMatrix();
	//glLoadIdentity();
	glScalef(1.25, 1, 1.25);
	glTranslatef(-0.42, -0.45, 0.05);
	glRotatef(-90, 0.0f, 0.0f, 1.0f);

	glScalef(0.25, 0.25, 0.25);
	glTranslatef(-1.6f, 0.0f, 0.0f);
	//Left, Right to rotate
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glRotatef(-initialArmRotate, 1.0f, 0.0f, 0.0f);

	glPushMatrix();
	glRotatef(5, 0.0, 1.0, 0.0);
	glColor3f(0, 0, 0);
	drawRec(-1.5f, 0.0f, 0.1f, 0.4f, 0.1f, 0.4f, GL_QUADS);
	glColor3f(1, 1, 1);
	drawRec(-0.8f, -1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	drawRec(-0.7f, -0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);

	glColor3f(0, 0, 0);
	drawRec(-1.5f, 0.0f, 0.1f, 0.4f, 0.1f, 0.4f, GL_LINE_LOOP);
	drawRec(-0.8f, -1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	drawRec(-0.7f, -0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);

	glColor3f(1, 1, 1);
	//shoulder
	glPushMatrix();
	glScalef(1, 1.4, 1);
	glTranslatef(0, -0.1, 0);
	glRotatef(60, 0.0, 1.0, 0.0);
	glTranslatef(0.6, 0.0, -1.6);
	drawRec(-1.0f, -1.9f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	glColor3f(0, 0, 0);
	drawRec(-1.0f, -1.9f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);

	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(-1.0, 0.5, 0.0);
	glRotatef(-90, 0.0, 0.0, 1.0);
	glRotatef(90, 1.0, 0.0, 1.0);
	drawPyramid(0, 0.5, 0, 0.5, 0.0, 0.5, 1, 2, GL_TRIANGLES, GL_QUADS);
	glColor3f(0, 0, 0);
	drawPyramid(0, 0.5, 0, 0.5, 0.0, 0.5, 1, 2, GL_LINE_LOOP, GL_LINE_LOOP);
	glColor3f(1, 1, 1);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.4, 0.2, 0.2);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawCylinder(0.0, 0.3, 0.8, GL_FILL);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();

	//Up, Down to move arm up down
	glRotatef(initialArmMove, 0.0f, 0.0f, 0.1f);
	initialArmMove += armMove;

	if (initialArmMove >= 60.0f) {

		initialArmMove = 60.0f;
		armMove = 0.0f;

	}
	else if (initialArmMove <= 0.0f) {

		initialArmMove = 0.0f;
		armMove = 0.0f;

	}
	glRotatef(initialLeftArmSpeed, 0.0f, 0.1f, 0.0f); //control lower arm

	initialLeftArmSpeed += leftArmSpeed;

	if (initialLeftArmSpeed >= leftArmMaxAngle) {
		initialLeftArmSpeed = leftArmMaxAngle;
		leftArmSpeed = 0.0f;
	}

	if (initialLeftArmSpeed <= leftArmMinAngle) {
		initialLeftArmSpeed = leftArmMinAngle;
		leftArmSpeed = 0.0f;
	}
	glPushMatrix();
	glRotatef(-5, 0.0, 1.0, 0.0);

	glColor3f(0.0f, 0.0f, 0.0f);
	drawRec(0.0f, 1.5f, 0.1f, 0.4f, 0.1f, 0.4f, GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

	drawRec(0.8f, 1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	drawRec(0.7f, 0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	drawRec(0.8f, 1.5f, 0.0f, 0.1f, 0.0f, 0.1f, GL_QUADS);

	glColor3f(0, 0, 0);
	drawRec(0.0f, 1.5f, 0.1f, 0.4f, 0.1f, 0.4f, GL_LINE_LOOP);
	drawRec(0.8f, 1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	drawRec(0.7f, 0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	drawRec(0.8f, 1.5f, 0.0f, 0.1f, 0.0f, 0.1f, GL_LINE_LOOP);
	glColor3f(1, 1, 1);

	//shield
	glPushMatrix();
	glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.5, -0.65, -1.0);
	glScalef(2, 2, 2);
	controlShield();
	glPopMatrix();

	//joint
	glPushMatrix();
	glTranslatef(0.0, 0.25, 0.25);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawSphere(0.25, GL_FILL);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.5, 0.25, 0.25);
	drawSphere(0.15, GL_FILL);
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	leftHand();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawRightHand() {
	glPushMatrix();
	//glLoadIdentity();
	glScalef(1.25, 1, 1.25);
	glTranslatef(0.42, -0.45, -0.05);
	glRotatef(-90, 0.0f, 0.0f, 1.0f);

	glScalef(0.25, 0.25, 0.25);
	glTranslatef(-1.6f, 0.0f, 0.0f);

	//Left, Right to rotate
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	glRotatef(initialArmRotate1, 1.0f, 0.0f, 0.0f);
	initialArmRotate1 += armRotate1;

	glPushMatrix();

	glRotatef(5, 0.0, 1.0, 0.0);
	glColor3f(0, 0, 0);
	drawRec(-1.5f, 0.0f, 0.1f, 0.4f, 0.1f, 0.4f, GL_QUADS);
	glColor3f(1, 1, 1);

	drawRec(-0.8f, -1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	drawRec(-0.7f, -0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	glPushMatrix();
	glColor3f(0, 0, 0);
	drawRec(-1.5f, 0.0f, 0.1f, 0.4f, 0.1f, 0.4f, GL_LINE_LOOP);
	drawRec(-0.8f, -1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	drawRec(-0.7f, -0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	glPopMatrix();
	glColor3f(1, 1, 1);

	//shoulder
	glPushMatrix();
	glScalef(1, 1.4, 1);
	glTranslatef(0, -0.1, 0);
	glRotatef(60, 0.0, 1.0, 0.0);
	glTranslatef(0.6, 0.0, -1.6);
	drawRec(-1.0f, -1.9f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	glColor3f(0, 0, 0);
	drawRec(-1.0f, -1.9f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(-1.0, 0.5, 0.0);
	glRotatef(-90, 0.0, 0.0, 1.0);
	glRotatef(90, 1.0, 0.0, 1.0);
	drawPyramid(0, 0.5, 0, 0.5, 0.0, 0.5, 1, 2, GL_TRIANGLES, GL_QUADS);
	glColor3f(0, 0, 0);
	drawPyramid(0, 0.5, 0, 0.5, 0.0, 0.5, 1, 2, GL_LINE_LOOP, GL_LINE_LOOP);
	glColor3f(1, 1, 1);
	glPopMatrix();

	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.4, 0.2, 0.2);
	glColor3f(0, 0, 0);
	drawCylinder(0.0, 0.3, 0.8, GL_FILL);

	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	//Up, Down to move arm up down
	glRotatef(-initialArmMove1, 0.0f, 0.0f, 0.1f);
	initialArmMove1 += armMove1;

	if (initialArmMove1 >= 90.0f) {
		initialArmMove1 = 90.0f;
		armMove1 = 0.0f;
	}
	else if (initialArmMove1 <= 0.0f) {
		initialArmMove1 = 0.0f;
		armMove1 = 0.0f;
	}

	glRotatef(initialRightArmSpeed, 0.0f, 0.1f, 0.0f); //control lower arm

	initialRightArmSpeed += rightArmSpeed;

	if (initialRightArmSpeed >= rightArmMaxAngle) {
		initialRightArmSpeed = rightArmMaxAngle;
		rightArmSpeed = 0.0f;
	}

	if (initialRightArmSpeed <= rightArmMinAngle) {
		initialRightArmSpeed = rightArmMinAngle;
		rightArmSpeed = 0.0f;
	}
	glPushMatrix();

	glRotatef(-5, 0.0, 1.0, 0.0);

	glColor3f(0, 0, 0);
	drawRec(0.0f, 1.5f, 0.1f, 0.4f, 0.1f, 0.4f, GL_QUADS);
	glColor3f(1, 1, 1);

	drawRec(0.8f, 1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	drawRec(0.7f, 0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_QUADS);
	drawRec(0.8f, 1.5f, 0.0f, 0.1f, 0.0f, 0.1f, GL_QUADS);
	glColor3f(0, 0, 0);
	drawRec(0.0f, 1.5f, 0.1f, 0.4f, 0.1f, 0.4f, GL_LINE_LOOP);
	drawRec(0.8f, 1.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	drawRec(0.7f, 0.2f, 0.0f, 0.5f, 0.0f, 0.5f, GL_LINE_LOOP);
	drawRec(0.8f, 1.5f, 0.0f, 0.1f, 0.0f, 0.1f, GL_LINE_LOOP);
	glColor3f(1, 1, 1);

	//shield
	glPushMatrix();
	glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.5, -0.65, -1.0);
	glScalef(2, 2, 2);
	controlShield();
	glPopMatrix();

	//joint
	glPushMatrix();
	glTranslatef(0.0, 0.25, 0.25);
	glColor3f(0, 0, 0);
	drawSphere(0.25, GL_FILL);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(1.5, 0.25, 0.25);
	drawSphere(0.15, GL_FILL);
	glPopMatrix();
	glColor3f(1, 1, 1);
	rightHand();


	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

//Shield
void DrawHandle() {

	glPushMatrix();
	drawRec(0.0f, 0.3f, 0.0f, 0.05f, 0.0f, 0.05f, GL_QUADS);
	glPopMatrix();

	glPushMatrix();
	glRotatef(60.0f, 0.0f, 0.0f, -1.0f);
	glTranslatef(-0.2f, 0.0f, 0.0f);

	drawRec(0.0f, 0.2f, 0.0f, 0.05f, 0.0f, 0.05f, GL_QUADS);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.3f, 0.0f, 0.0f);
	glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.3f, -0.0f, -0.0f);

	glTranslatef(0.3f, 0.0f, 0.0f);

	drawRec(0.0f, 0.2f, 0.0f, 0.05f, 0.0f, 0.05f, GL_QUADS);
	glPopMatrix();

}

void DrawShield() {
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.15f, 0.0f, 0.2f);
	glScalef(2.0f, 2.0f, 2.0f);
	shieldPentagon(0.0f, 0.04f, GL_POLYGON, GL_QUADS);
	glColor3f(0, 0, 0);
	shieldPentagon(0.0f, 0.04f, GL_LINE_LOOP, GL_LINE_LOOP);
	glColor3f(1, 1, 1);
	glPopMatrix();
}

void shieldPentagon(float minZ, float maxZ, GLenum type1, GLenum type2) {

	glBegin(type1);
	glTexCoord2f(-0.1f, minZ);
	glVertex3f(-0.1f, 0.2f, minZ);
	glTexCoord2f(0.1f, minZ);
	glVertex3f(0.1f, 0.2f, minZ);
	glTexCoord2f(0.16f, minZ);
	glVertex3f(0.16f, -0.09f, minZ);
	glTexCoord2f(0.0f, minZ);
	glVertex3f(0.0f, -0.25f, minZ);
	glTexCoord2f(-0.16f, minZ);
	glVertex3f(-0.16f, -0.09f, minZ);
	glEnd();

	glBegin(type1);
	glTexCoord2f(-0.1f, maxZ);
	glVertex3f(-0.1f, 0.2f, maxZ);
	glTexCoord2f(0.1f, maxZ);
	glVertex3f(0.1f, 0.2f, maxZ);
	glTexCoord2f(0.16f, maxZ);
	glVertex3f(0.16f, -0.09f, maxZ);
	glTexCoord2f(0.0f, maxZ);
	glVertex3f(0.0f, -0.25f, maxZ);
	glTexCoord2f(-0.16f, maxZ);
	glVertex3f(-0.16f, -0.09f, maxZ);
	glEnd();

	glBegin(type2);
	glTexCoord2f(-0.16f, minZ);
	glVertex3f(-0.16f, -0.09f, minZ);
	glTexCoord2f(0.0f, minZ);
	glVertex3f(0.0f, -0.25f, minZ);
	glTexCoord2f(0.0f, maxZ);
	glVertex3f(0.0f, -0.25f, maxZ);
	glTexCoord2f(-0.16f, maxZ);
	glVertex3f(-0.16f, -0.09f, maxZ);
	glEnd();

	glBegin(type2);
	glTexCoord2f(-0.16f, minZ);
	glVertex3f(-0.16f, -0.09f, minZ);
	glTexCoord2f(-0.16f, maxZ);
	glVertex3f(-0.16f, -0.09f, maxZ);
	glTexCoord2f(-0.1f, maxZ);
	glVertex3f(-0.1f, 0.2f, maxZ);
	glTexCoord2f(-0.1f, minZ);
	glVertex3f(-0.1f, 0.2f, minZ);
	glEnd();

	glBegin(type2);
	glTexCoord2f(-0.1f, minZ);
	glVertex3f(-0.1f, 0.2f, minZ);
	glTexCoord2f(-0.1f, maxZ);
	glVertex3f(-0.1f, 0.2f, maxZ);
	glTexCoord2f(0.1f, maxZ);
	glVertex3f(0.1f, 0.2f, maxZ);
	glTexCoord2f(0.1f, minZ);
	glVertex3f(0.1f, 0.2f, minZ);
	glEnd();

	glBegin(type2);
	glTexCoord2f(0.1f, minZ);
	glVertex3f(0.1f, 0.2f, minZ);
	glTexCoord2f(0.1f, maxZ);
	glVertex3f(0.1f, 0.2f, maxZ);
	glTexCoord2f(0.16f, maxZ);
	glVertex3f(0.16f, -0.09f, maxZ);
	glTexCoord2f(0.16f, minZ);
	glVertex3f(0.16f, -0.09f, minZ);
	glEnd();

	glBegin(type2);
	glTexCoord2f(0.16f, minZ);
	glVertex3f(0.16f, -0.09f, minZ);
	glTexCoord2f(0.16f, maxZ);
	glVertex3f(0.16f, -0.09f, maxZ);
	glTexCoord2f(0.0f, maxZ);
	glVertex3f(0.0f, -0.25f, maxZ);
	glTexCoord2f(0.0f, minZ);
	glVertex3f(0.0f, -0.25f, minZ);
	glEnd();

}

void controlShield() {

	glPushMatrix();
	glTranslatef(-0.2, 0.23f, -0.15f);
	glScalef(0.65f, 0.65f, 0.65f);

	DrawHandle();
	DrawShield();
	glPopMatrix();

}

void drawBody()
{
	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glTranslatef(0, -1.5, 0);
	//neck
	glPushMatrix();
	glTranslatef(0, 4, 0.05);
	color(0, 0, 0);
	cylinder(0.45, 1, 0.35, 0.5);
	glPopMatrix();
	//inner chest
	glPushMatrix();
	glTranslatef(0, 2, 0);
	glScalef(1.5, 1, 0.8);
	glScalef(1, -1, 1);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_QUADS);
	glPopMatrix();

	//outer chest top1
	glPushMatrix();
	glTranslatef(0, 2.8, 0);
	glScalef(1.85, 0.4, 0.9);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.15, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.15, GL_LINE_STRIP);
	glPopMatrix();
	//outer chest top2
	glPushMatrix();
	glTranslatef(0, 2.3, 0);
	glScalef(2, -0.6, 1);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_LINE_STRIP);
	glPopMatrix();

	//power core
	glPushMatrix();
	glTranslatef(0, 2.1, 0.2);
	glRotated(80, 1, 0, 0);
	color(255, 127, 80);
	cylinder(0.25, 0.8, 0.25, 1);
	glPopMatrix();

	//outer chest btm
	glPushMatrix();
	glTranslatef(0, 1.6, 0);
	glScalef(1.5, -0.5, 0.8);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_LINE_STRIP);
	glPopMatrix();

	//outer body top
	glPushMatrix();
	glTranslatef(0, 1.2, 0);
	glScalef(1.2, -0.5, 0.8);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();

	//outer body btm 1
	glPushMatrix();
	glTranslatef(0, 0.6, 0);
	glScalef(1.2, -0.25, 0.8);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();

	//outer body btm 2
	glPushMatrix();
	glTranslatef(0, 0.4, 0);
	glScalef(1.5, -0.3, 1);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();

	//inner body
	glPushMatrix();
	glTranslatef(0, 1, 0);
	glScalef(1, 1.5, 0.6);
	color(0, 0, 0);
	drawRec(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, GL_QUADS);
	glPopMatrix();

	//below body
	glPushMatrix();
	glTranslatef(0, 0.15, 0);
	glScalef(0.4, 0.8, 0.8);
	color(255, 255, 255);
	drawRec(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();

	//left leg joint
	glPushMatrix();
	glRotated(-120, 0, 0, 1);
	glTranslatef(-0.2, 0.8, 0);
	color(0, 0, 0);
	cylinder(0.3, 0.8, 0.3, 0);
	glPopMatrix();
	//glPushMatrix();
	//glTranslatef(0.75, -0.2, 0);
	//color(0, 0, 0);
	//drawSphere(0.25, GL_FILL);
	//glPopMatrix();

	//right leg joint
	glPushMatrix();
	glRotated(120, 0, 0, 1);
	glTranslatef(0.2, 0.8, 0);
	color(0, 0, 0);
	cylinder(0.3, 0.8, 0.3, 0);
	glPopMatrix();
	//glPushMatrix();
	//glTranslatef(-0.75, -0.2, 0);
	//color(0, 0, 0);
	//drawSphere(0.25, GL_FILL);
	//glPopMatrix();

	//jetpack
	glPushMatrix();
	glTranslatef(0, 2.2, 0.6);
	glRotated(92, 1, 0, 0);
	glPushMatrix();
	glScaled(0.52, 0.3, 0.5);
	//main thruster
	glPushMatrix();
	glTranslatef(0, 0.8, 0);
	color(0, 0, 0);
	cylinder(0.5, 0.6, 0.5, 1);
	glPopMatrix();
	//left laser
	glPushMatrix();
	glScaled(1, -1, 1);
	glRotated(65, 0, 1, 0);
	glTranslatef(3, 0, 0.2);
	glScaled(3, 1, 0.6);
	color(255, 255, 255);
	triangle(GL_POLYGON);
	color(0, 0, 0);
	triangle(GL_LINE_LOOP);
	glPushMatrix();
	glScaled(0.2, -2, 1);
	glTranslatef(-0.5, 0, 0.6);
	color(0, 0, 0);
	cylinder(0.5, 0.8, 0.4, 1);
	glPopMatrix();
	glPopMatrix();
	//right laser
	glPushMatrix();
	glScaled(-1, -1, 1);
	glRotated(65, 0, 1, 0);
	glTranslatef(3, 0, 0.2);
	glScaled(3, 1, 0.6);
	color(255, 255, 255);
	triangle(GL_POLYGON);
	color(0, 0, 0);
	triangle(GL_LINE_LOOP);
	glPushMatrix();
	glScaled(0.2, -2, 1);
	glTranslatef(-0.5, 0, 0.5);
	color(0, 0, 0);
	cylinder(0.5, 0.8, 0.4, 1);
	glPopMatrix();
	glPopMatrix();

	//BTM QUAD
	glPushMatrix();
	glTranslatef(0, 0.2, 1.3);
	//sub thruster
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslatef(0, 0.7, 0);
	color(0, 0, 0);
	cylinder(0.1, 1.4, 0.1, 1);
	//right thruster
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glTranslatef(0, 0.15, 0);
	color(0, 0, 0);
	cylinder(0.5, 0.8, 0.4, 0);
	glPopMatrix();
	//left thruster
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glTranslatef(0, 0.15, -1.4);
	color(0, 0, 0);
	cylinder(0.5, 0.8, 0.4, 0);
	glPopMatrix();
	glPopMatrix();
	glScaled(0.5, 1, 1);
	glRotated(50, 1, 0, 0);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.1, GL_LINE_LOOP);
	glPopMatrix();
	//right board
	glPushMatrix();
	glTranslatef(-0.5, -0.4, 1.5);
	glRotated(60, 0, 1, 0);
	glScaled(3, 0.2, 0.9);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0, GL_LINE_LOOP);
	glPopMatrix();
	//left board
	glPushMatrix();
	glScaled(-1, 1, 1);
	glTranslatef(-0.5, -0.4, 1.5);
	glRotated(60, 0, 1, 0);
	glScaled(3, 0.2, 0.9);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0, GL_LINE_LOOP);
	glPopMatrix();
	//QUAD
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 1, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 1, GL_LINE_LOOP);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix(); // end of whole body
}

void drawHead()
{
	glPushMatrix();
	glScalef(0.75, 1, 1);
	glPushMatrix();
	glScalef(0.2, 0.2, 0.2);
	glScalef(0.85, 0.9, 0.9);
	glTranslatef(0, 2.5, 0.05);
	//back clown
	glPushMatrix();
	glTranslatef(0.9, 0.45, -0.1);
	glPushMatrix();
	glRotated(-41, 0, 1, 0);
	glRotated(-15, 0, 0, 1);
	glScaled(1.1, 1.5, 0.1);
	color(0, 0, 0);
	triangle(GL_POLYGON);
	color(0, 0, 0);
	triangle(GL_LINE_LOOP);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.9, 0.45, -0.1);
	glPushMatrix();
	glScaled(-1, 1, 1);
	glRotated(-41, 0, 1, 0);
	glRotated(-15, 0, 0, 1);
	glScaled(1.1, 1.5, 0.1);
	color(0, 0, 0);
	triangle(GL_POLYGON);
	color(0, 0, 0);
	triangle(GL_LINE_LOOP);
	glPopMatrix();
	glPopMatrix();
	//front clown
	glPushMatrix();
	glPushMatrix();
	glTranslatef(1.2, 0.7, 0);
	glPushMatrix();
	glRotated(-41, 0, 1, 0);
	glScaled(1.6, 0.6, 0.1);
	color(255, 255, 255);
	triangle(GL_POLYGON);
	color(0, 0, 0);
	triangle(GL_LINE_LOOP);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.2, 0.7, 0);
	glPushMatrix();
	glScaled(-1, 1, 1);
	glRotated(-41, 0, 1, 0);
	glScaled(1.6, 0.6, 0.1);
	color(255, 255, 255);
	triangle(GL_POLYGON);
	color(0, 0, 0);
	triangle(GL_LINE_LOOP);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	//ear
	glPushMatrix();
	glTranslatef(0, 0.25, 0.1);
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	color(0, 0, 0);
	glPushMatrix();
	glTranslatef(0, 1, 0);
	cylinder(0.42, 2, 0.32, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 1.15, 0);
	color(255, 255, 255);
	cylinder(0.3, 2.3, 0.2, 1);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	//top head
	glPushMatrix();
	glScaled(0.35, 0.8, 0.35);
	glTranslatef(0, 0.2, 0);
	glPushMatrix();
	glRotated(45, 0, 1, 0);
	glTranslatef(0, 1, 0);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 1.5, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 1.5, GL_LINE_STRIP);
	glPopMatrix();
	glPopMatrix();
	// horn
	glPushMatrix();
	glTranslatef(0, 0.6, -0.2);
	glScalef(1.4, 0.2, 1.2);
	glPushMatrix();
	glRotated(45, 0, 1, 0);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();
	glPopMatrix();
	//front head
	glPushMatrix();
	glScaled(1.2, 1, 1);
	glPushMatrix();
	glRotated(-20, 1, 0, 0);
	glRotated(45, 0, 1, 0);
	glTranslatef(0, 0.6, 0);
	glScalef(1, 0.5, 1);
	color(0, 0, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	glPopMatrix();
	glPushMatrix();
	glRotated(-20, 1, 0, 0);
	glRotated(45, 0, 1, 0);
	glTranslatef(0, 0.1, 0);
	glScalef(1, 0.5, 1);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();
	glPopMatrix();
	//back head
	glPushMatrix();
	glScaled(1.2, 1, 0.9);
	glTranslatef(0, 0.1, 0.1);
	glPushMatrix();
	glRotated(20, 1, 0, 0);
	glRotated(45, 0, 1, 0);
	glTranslatef(0, 0.6, 0);
	glScalef(1, 0.5, 1);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();
	glPushMatrix();
	glRotated(20, 1, 0, 0);
	glRotated(45, 0, 1, 0);
	glTranslatef(0, 0.05, 0);
	glScalef(1, 0.6, 1);
	color(255, 255, 255);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_QUADS);
	color(0, 0, 0);
	drawQuad(-0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.0, GL_LINE_STRIP);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawCone(double br, double h) {
	GLUquadricObj* cylinder = NULL;
	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_LINE);
	glColor3f(0.8, 0.0, 0.0);
	gluCylinder(cylinder, 0.5, br, h, 30, 30);
	gluDeleteQuadric(cylinder);
}

void drawConeShape(double br, double h) {
	GLUquadricObj* cylinder = NULL;
	cylinder = gluNewQuadric();
	glColor3f(1, 0.5, 0);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricTexture(cylinder, true);
	gluCylinder(cylinder, 0.5, br, h, 30, 30);
	gluDeleteQuadric(cylinder);
}

void drawScope(double r) {
	GLUquadricObj* sphere = NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, true);
	gluSphere(sphere, r, 20, 20);
	gluDeleteQuadric(sphere);
}

void drawThighUp(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Down Front Left
	glPushMatrix();
	glScalef(1.4f, 1.4f, 1.4f);
	glTranslatef(-0.2f, 0.0f, -0.2f);
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.465f, 0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 1.465f, 0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, 1.465f, 0.0f);
	glEnd();

	//Down Front Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, 0.465f, 0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, 1.465f, 0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, 1.465f, 0.0f);
	glEnd();

	//Down Back Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 0.0f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.465f, 0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 1.465f, 0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, 1.465f, 1.6f);
	glEnd();

	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 0.0f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, 0.465f, 0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, 1.465f, 0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, 1.465f, 1.6f);
	glEnd();

	glPopMatrix();

	//Up
	glPushMatrix();
	glScalef(1.4f, 1.2f, 1.4f);
	glTranslatef(-0.2f, 0.25f, -0.2f);

	//Up Front Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 1.465f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, 1.465f, 0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.26f, 2.725f, 0.8f);
	glEnd();

	//Up Front Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 1.465f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 1.465f, 0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.26f, 2.725f, 0.8f);
	glEnd();

	//Up back Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 1.465f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 1.465f, 0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.26f, 2.725f, 0.8f);
	glEnd();

	//Up Back Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, 1.465f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, 1.465f, 0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.26f, 2.725f, 0.8f);
	glEnd();
	glPopMatrix();

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.8f, 0.4f, 0.8f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluCylinder(var, 0.75f, 0.75f, 0.8f, 10, 10);
	glColor3f(r, g, b);
	glPopMatrix();

	glPopMatrix();
}

void drawThighDown(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();
	//Front Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -0.75f, 0.12f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -0.75f, 0.245f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, 0.165f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, -0.3f, 0.0f);
	glEnd();

	//Front Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -0.75f, 0.12f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -0.75f, 0.245f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.165f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, -0.3f, 0.0f);
	glEnd();

	//Back Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -0.75f, 1.48f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -0.75f, 1.365f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, 0.165f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, -0.3f, 1.6f);
	glEnd();


	//Back Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -0.75f, 1.48f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -0.75f, 1.365f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.165f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, -0.3f, 1.6f);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -0.75f, 0.245f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -0.75f, 1.365f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, 0.165f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, 0.165f, 0.0f);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.165f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.75f, 0.245f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.75f, 1.365f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.165f, 1.6f);
	glEnd();

	//Top Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -0.3f, 0.0f);
	glTexCoord2f(0.0f, 0.1f);
	glVertex3f(0.0f, 0.165f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.165f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, -0.3f, 1.6f);
	glEnd();

	//Top Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, 0.165f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, 0.165f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, -0.3f, 1.6f);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -0.75f, 0.245f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -0.75f, 0.245f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -0.75f, 1.365f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.75f, 1.365f);
	glEnd();

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(-0.1f, -0.75f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluCylinder(var, 0.5f, 0.5f, 1.8f, 10, 10);
	glColor3f(r, g, b);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(-0.1f, -0.75f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.5f * cosf(theta);//calculate the x component
		float z = 0.5f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glColor3f(r, g, b);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.11f, -0.75f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.3f * cosf(theta);//calculate the x component
		float z = 0.3f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(1.7f, -0.75f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.5f * cosf(theta);//calculate the x component
		float z = 0.5f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glColor3f(r, g, b);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.71f, -0.75f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.3f * cosf(theta);//calculate the x component
		float z = 0.3f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glEnd();
	glPopMatrix();

	glPopMatrix();
}

void drawCalfUpSideArmor(GLenum type, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -1.75f, 0.8f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -3.75f, 0.325f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -3.75f, 1.285f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 1.6f);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.2f, -1.75f, 0.8f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.2f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.2f, -3.75f, 0.325f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.2f, -3.75f, 1.285f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.2f, -2.55f, 1.6f);
	glEnd();

	//Front Up
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -1.75f, 0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.2f, -1.75f, 0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.2f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.0f, -2.55f, 0.0f);
	glEnd();

	//Front Down
	glBegin(type);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.0f, -2.55f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.2f, -2.55f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.2f, -3.75f, 0.325);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -3.75f, 0.325f);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -3.75f, 0.325f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.2f, -3.75f, 0.325);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.2f, -3.75f, 1.285f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -3.75f, 1.285f);
	glEnd();

	//Back Down
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -3.75f, 1.285f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.2f, -3.75f, 1.285f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.2f, -2.55f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 1.6f);
	glEnd();

	glBegin(type);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.2f, -2.55f, 1.6f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.2f, -1.75f, 0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -1.75f, 0.8f);
	glEnd();

	glPopMatrix();
}

void drawCalfUpFrontArmor(GLenum type, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Up Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -1.165f, -0.375f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.34f, -1.585f, -0.265f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, -1.585f, -0.8f);
	glEnd();

	//Up Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -1.165f, -0.375f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.265f, -1.585f, -0.265f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, -1.585f, -0.8f);
	glEnd();

	//Up Back
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -1.165f, -0.375f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.265f, -1.585f, -0.265f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.34f, -1.585f, -0.265f);
	glEnd();

	//Middle Right
	glBegin(type);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.34f, -1.585f, -0.265f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -1.585f, -0.8f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8f, -2.55f, -0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -2.55f, 0.0f);
	glEnd();

	//Middle Left
	glBegin(type);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.265f, -1.585f, -0.265f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -1.585f, -0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8f, -2.55f, -0.8f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 0.0f);
	glEnd();

	//Middle Up Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -2.55f, -0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -3.15f, 0.16f);
	glEnd();

	//Middle Up Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -2.55f, -0.8f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -3.15f, 0.16f);
	glEnd();

	//Middle Down Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -2.55f, -0.8f);
	glTexCoord2f(0.0f, 0.1f);
	glVertex3f(0.8f, -3.605f, 0.28f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -3.15f, 0.16f);
	glEnd();

	//Middle Down Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8f, -2.55f, -0.8f);
	glTexCoord2f(0.0f, 0.1f);
	glVertex3f(0.8f, -3.605f, 0.28f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -3.15f, 0.16f);
	glEnd();

	glPopMatrix();
}

void drawCalfUp(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.8f, -1.5f, 0.8f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluSphere(var, 0.75f, 30, 30);
	glColor3f(r, g, b);
	glPopMatrix();

	//Front
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -1.75f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -2.55f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -1.75f, 0.0f);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -2.55f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -1.75f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -1.75f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -2.55f, 1.6f);
	glEnd();

	//Back
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -1.75f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -2.55f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -1.75f, 1.6f);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -2.55f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -1.75f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -1.75f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -2.55f, 0.0f);
	glEnd();

	//Top
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -1.75f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -1.75f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -1.75f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -1.75f, 0.0f);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -2.55f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -2.55f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -2.55f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -2.55f, 0.0f);
	glEnd();

	drawCalfUpSideArmor(type, r, g, b);

	glPushMatrix();
	glTranslatef(1.8f, 0.0f, 0.0f);
	drawCalfUpSideArmor(type, r, g, b);
	glPopMatrix();

	drawCalfUpFrontArmor(type, r, g, b);

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.8f, -2.6f, 0.8f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluCylinder(var, 0.75f, 0.75f, 1.4f, 10, 10);
	glColor3f(r, g, b);
	glPopMatrix();

	glPopMatrix();
}

void drawCalfMiddle(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Front
	glBegin(type);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.8f, -4.065f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -3.605f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -4.805f, 0.0f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.8f, -5.265f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -4.805f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -3.605f, 0.0f);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -3.605f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -4.805f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -4.805f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -3.605f, 1.6f);
	glEnd();

	//Back
	glBegin(type);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.8f, -4.065f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -3.605f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -4.805f, 1.6f);
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.8f, -5.265f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -4.805f, 1.6f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -3.605f, 1.6f);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -3.605f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -4.805f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -4.805f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -3.605f, 1.6f);
	glEnd();

	//Top Right
	glBegin(type);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -3.605f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -3.605f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8, -4.065f, 1.6f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8, -4.065f, 0.0f);
	glEnd();

	//Top Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -3.605f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -3.605f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.8, -4.065f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8, -4.065f, 0.0f);
	glEnd();

	//Bottom Right
	glBegin(type);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -4.805f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -4.805f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8, -5.265f, 1.6f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.8, -5.265f, 0.0f);
	glEnd();

	//Bottom Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -4.805f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -4.805f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.8, -5.265f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.8, -5.265f, 0.0f);
	glEnd();

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.8f, -5.0f, 0.8f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluCylinder(var, 0.75f, 0.75f, 0.8f, 10, 10);
	glColor3f(r, g, b);
	glPopMatrix();

	glPopMatrix();
}

void drawCalfDown(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Front
	glBegin(type);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.8f, -5.665f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -5.205f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -6.845f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -6.845f, 0.0f);
	glTexCoord2f(0.0f, 0.3f);
	glVertex3f(0.0f, -5.205f, 0.0f);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -5.205f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -6.845f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -6.845f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -5.205f, 1.6f);
	glEnd();

	//Back
	glBegin(type);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.8f, -5.665f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -5.205f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -6.845f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -6.845f, 1.6f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -5.205f, 1.6f);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -5.205f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -6.845f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -6.845f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -5.205f, 1.6f);
	glEnd();

	//Top Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -5.205f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -5.205f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.8, -5.665f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8, -5.665f, 0.0f);
	glEnd();

	//Top Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -5.205f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -5.205f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.8, -5.665f, 1.6f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.8, -5.665f, 0.0f);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -6.845f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -6.845f, 1.6f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -6.845f, 1.6f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -6.845f, 0.0f);
	glEnd();

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(-0.1f, -6.845f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluCylinder(var, 0.7f, 0.7f, 1.8f, 10, 10);
	glColor3f(r, g, b);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(-0.1f, -6.845f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.75f * cosf(theta);//calculate the x component
		float z = 0.75f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glColor3f(r, g, b);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.11f, -6.845f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.55f * cosf(theta);//calculate the x component
		float z = 0.55f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(1.7f, -6.845f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.75f * cosf(theta);//calculate the x component
		float z = 0.75f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glColor3f(r, g, b);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.71f, -6.845f, 0.8f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glBegin(type);
	for (int i = 0; i < 100; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

		float x = 0.55f * cosf(theta);//calculate the x component
		float z = 0.55f * sinf(theta);//calculate the y component

		glVertex3f(x + 0, z + 0, 0);//output vertex
	}
	glEnd();
	glPopMatrix();

	glPopMatrix();
}

void drawInSideHeel(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Front Right
	glBegin(type);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(0.8f, -7.775f, -0.45f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.6f, -7.445f, 0.28f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -7.945f, 0.28f);
	glEnd();

	//Front Left
	glBegin(type);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(0.8f, -7.775f, -0.45f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -7.445f, 0.28f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -7.945f, 0.28f);
	glEnd();

	//Front Bottom
	glBegin(type);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(0.8f, -7.775f, -0.45f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -7.945f, 0.28f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -7.945f, 0.28f);
	glEnd();

	glPopMatrix();
}

void drawOutSideHeel(GLenum type, GLenum gluType, float r, float g, float b)
{
	glColor3f(r, g, b);
	glPushMatrix();

	//Framework
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.8f, -7.775f, 0.8f);
	var = gluNewQuadric();
	gluQuadricDrawStyle(var, gluType);
	gluSphere(var, 0.75f, 30, 30);
	glColor3f(r, g, b);
	glPopMatrix();

	//Front Up
	glBegin(type);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(0.8f, -7.775f, -0.45f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -7.445f, 0.28f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -8.545f, -2.12f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -8.545f, -2.12f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -7.445f, 0.28f);
	glEnd();

	//Front Down
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -8.545f, -2.12f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -8.545f, -2.12f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -8.945f, -2.12f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -8.945f, -2.12f);
	glEnd();

	//Right
	glBegin(type);
	glTexCoord2f(0.0f, 0.8f);
	glVertex3f(1.6f, -8.545f, -2.12f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.6f, -8.945f, -2.12f);
	glTexCoord2f(0.8f, 1.0f);
	glVertex3f(1.6f, -8.945f, 1.82f);
	glTexCoord2f(1.0f, 0.3f);
	glVertex3f(1.6f, -8.155f, 2.035f);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(1.6f, -7.445f, 1.325f);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(1.6f, -7.445f, 0.28f);
	glEnd();

	//Back Up
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -7.445f, 1.325f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -7.445f, 1.325f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -8.155f, 2.035f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -8.155f, 2.035f);
	glEnd();

	//Back Down
	glBegin(type);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -8.155f, 2.035f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -8.155f, 2.035f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -8.945f, 1.825f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -8.945f, 1.825f);
	glEnd();

	//Left
	glBegin(type);
	glTexCoord2f(0.0f, 0.8f);
	glVertex3f(0.0f, -8.545f, -2.12f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -8.945f, -2.12f);
	glTexCoord2f(0.8f, 1.0f);
	glVertex3f(0.0f, -8.945f, 1.82f);
	glTexCoord2f(1.0f, 0.3f);
	glVertex3f(0.0f, -8.155f, 2.035f);
	glTexCoord2f(0.4f, 0.0f);
	glVertex3f(0.0f, -7.445f, 1.325f);
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.0f, -7.445f, 0.28f);
	glEnd();

	//Bottom
	glBegin(type);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, -8.945f, -2.12f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.6f, -8.945f, -2.12f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.6f, -8.945f, 1.825f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, -8.945f, 1.825f);
	glEnd();

	drawInSideHeel(type, gluType, r, g, b);

	glPopMatrix();
}

void drawLeg()
{
	glPushMatrix();
	glScalef(0.1f, 0.08f, 0.1f);
	glTranslatef(-2.8f, -7.5f, -0.8f);
	//glTranslatef(0.0, 1.6f * thighAngle / 45, -1.6f * thighAngle / 45);

	glPushMatrix();
	//glRotatef(thighAngle, 1.0f, 0.0f,0.0f);


	drawThighUp(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawThighUp(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	drawThighDown(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawThighDown(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	drawCalfUp(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawCalfUp(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	drawCalfMiddle(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawCalfMiddle(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	drawCalfDown(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawCalfDown(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glPushMatrix();
	drawOutSideHeel(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);;
	drawOutSideHeel(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
	glScalef(-0.1f, 0.08f, 0.1f);
	glTranslatef(-2.8f, -7.5f, -0.8f);

	glPushMatrix();
	drawThighUp(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawThighUp(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	drawThighDown(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawThighDown(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	drawCalfUp(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawCalfUp(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	drawCalfMiddle(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawCalfMiddle(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	drawCalfDown(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);
	drawCalfDown(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glPushMatrix();
	drawOutSideHeel(GL_LINE_LOOP, GLU_LINE, 0.0f, 0.0f, 0.0f);;
	drawOutSideHeel(GL_POLYGON, GLU_FILL, 1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glPopMatrix();
}

void loadAllTexture()
{
	for (int i = 0; i < 8; i++)
	{
		str = "textures/armor" + std::to_string(i + 1) + ".bmp";
		armorTexture[i] = loadTexture(str.c_str());
	}
	//if (isTexture == true) {
	//	switch (textureType) {
	//	case 1:
	//		str = "textures/armor_texture_1.bmp";
	//		break;
	//	case 2:
	//		str = "textures/cone.bmp";
	//		break;
	//	case 3:
	//		str = "textures/Skin1.bmp";
	//		break;
	//	case 4:
	//		str = "textures/Skin2.bmp";
	//		break;
	//	case 5:
	//		str = "textures/Skin3.bmp";
	//		break;
	//	default:
	//		break;
	//	}
	//	textures = loadTexture(str.c_str());
	//}
	//else
	//	glDeleteTextures(1, &textures);

}
void lighting() {

	if (isLightOn) {
		glEnable(GL_LIGHTING);
	}
	else {
		glDisable(GL_LIGHTING);
	}
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_POSITION, posA);
	glEnable(GL_LIGHT1); // turn on light source

	glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT1, GL_POSITION, posD);
	glEnable(GL_LIGHT1); // turn on light source

}
void display() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(135 / 255.0f, 206 / 255.0f, 235 / 255.0f, 0.0f);
	lighting();
	//camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (!switchCamera)
	{
		glOrtho(-10, 10, -10, 10, oNear, oFar);
		glTranslatef(0, 0, -8);
		glRotated(10, 1, 0, 0);
		glScaled(8, 8, -8);
	}
	else
	{
		gluPerspective(90, 1, 1, -1);
		glFrustum(-1, 1, -1, 1, pZMin, pZMax);
	}

	glBindTexture(GL_TEXTURE_2D, armorTexture[textureType]);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotated(bodyRotateY, 0, 1, 0);
	glRotated(bodyRotateX, 1, 0, 0);
	glTranslatef(0, 0.3, zoom);
	glTranslatef(pTx, pTy, 0);

	drawHead();
	drawBody();
	drawLeftHand();
	drawRightHand();
	drawLeg();
	glPopMatrix();

	//Step 5 :remove texture info

}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	var = gluNewQuadric();
	loadAllTexture();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		display();

		SwapBuffers(hdc);
	}

	gluDeleteQuadric(var);
	glDisable(GL_TEXTURE_2D);
	DeleteObject(hBMP);
	for (int i = 0; i < 8; i++)
	{
		glDeleteTextures(1, &armorTexture[i]);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}