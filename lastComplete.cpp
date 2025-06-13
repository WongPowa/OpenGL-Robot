
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>

#define WINDOW_TITLE "Robot"

int qNo = 0;
char rotate = ' ';

float viewAngleY = 1;
float viewAngleX = 1;
float rotateX = 1.0, rotateY = 1.0, rotateZ = 1.0;

bool isDorsiFlexion = true;
bool isMaxLegAngle = false;

float rightLegAngle = 0;
float rightAnkleDorsiFlexion = 20;
float rightAnklePlantarFlexion = -25;
float rightAnkleAngle = 0;

float leftLegAngle = 0;
float leftAnkleDorsiFlexion = rightAnkleDorsiFlexion;
float leftAnklePlantarFlexion = rightAnklePlantarFlexion;
float leftAnkleAngle = 0;

float kneeAngle = 0;

const float PI = 3.1415;
float legFanAngle = 0;

bool canRotate = false;

//arm stuff
bool isLeftArmRotateFront = true, isSubLeftArmRotateFront = true;
float leftArmJoint = 0.0f, subLeftArmJoint = 0.0f;
bool isRightArmRotateFront = false, isSubRightArmRotateFront = true;
float rightArmJoint = 0.0f, subRightArmJoint = 0.0f;
bool isFingerClench = true;
float firstFingerJoint = 0.0f, secondFingerJoint = 0.0f, thirdFingerJoint = 0.0f;
float maxFingerAngle = 90.0f;

bool isWeapon = true;

bool isShoulderYFinished = false, isShoulderZFinished = false;
float rotateShoulderToGrabYAxis = 0.0f, rotateShoulderToGrabZAxis = 0.0f, rotateElbowToGrabXAxis = 0.0f, rotateElbowToGrabZAxis = 0.0f;

bool weaponActivated = false, attackPhase = false, swordOut = false;
float rotateShoulderToSwingXAxis = 0.0f;

//move
float tx = 0, ty = 0, tz = 0;

//projection
bool isOrtho = true;

float ptx = 0.0f, pty = 0.0f; // translation for the projection 
float ptSpeed = 0.1f;
float tSpeed = 0.01f, orthoNear = 4.0f, orthoFar = -4.0f, perspecNear = 1.0f, perspecFar = 21.0f, s1Rad = 1.0f;
float pRy = 0.0f, prSpeed = 1.0f;

//test lighting
float amb[4] = { 1.0f, 0.0f, 0.0f, 0.1f };

float posA[3] = { 0, 0 , 0 };

float posD[3] = { 0.8f, 0.0f, 0.0f };

float dif[3] = { 1.0f, 0.0f, 0.0f };

float ambM[3] = { 0.0f, 0.0f, 1.0f };	//M is materials // red ambient material

float difM[3] = { 1.0f, 1.0f, 0.0f };

bool isLightOn = false;


//Texture
BITMAP BMP;
HBITMAP hBMP = NULL;

bool canloadTexture = false;	

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		else if (wParam == 0x57) //W Key
		{
			if (isOrtho)
			{
				if (tz > orthoFar + 2.0)
				{
					tz -= tSpeed;
				}
			}
			else
			{
				if (tz > perspecNear + s1Rad) // perspective near boundary
				{
					tz -= tSpeed;
				}
			}
			if (!isMaxLegAngle) //limits the leg turning angle to 30 and -30 degrees
			{
				rightLegAngle++;
				leftLegAngle--;
				kneeAngle--;

				if (rightLegAngle >= 30)
				{
					isMaxLegAngle = !isMaxLegAngle;
				}
			}
			else {
				rightLegAngle--;
				leftLegAngle++;

				if (rightLegAngle <= -30)
				{
					isMaxLegAngle = !isMaxLegAngle;
				}
			}

			if (isDorsiFlexion) //limits the ankle rotation
			{
				if (rightAnkleAngle < rightAnkleDorsiFlexion) {
					rightAnkleAngle++;
				}
				else if (rightAnkleAngle == rightAnkleDorsiFlexion)
				{
					isDorsiFlexion = !isDorsiFlexion;
				}

				if (leftAnkleAngle > leftAnklePlantarFlexion)
				{
					leftAnkleAngle--;
				} //will auto change thanks to the if statement above

			}
			else { //dorsiflexion false
				if (rightAnkleAngle > rightAnklePlantarFlexion) {
					rightAnkleAngle--;
				}
				else if (rightAnkleAngle == rightAnklePlantarFlexion)
				{
					isDorsiFlexion = !isDorsiFlexion;
				}

				if (leftAnkleAngle < leftAnkleDorsiFlexion)
				{
					leftAnkleAngle++;
				} //will auto change thanks to the if statement above
			}

			if (isLeftArmRotateFront == true)
			{
				leftArmJoint++;
				if (isSubLeftArmRotateFront == true)
				{
					subLeftArmJoint++;
					if (leftArmJoint >= 30)
					{
						isLeftArmRotateFront = false;
					}
					if (subLeftArmJoint >= 30)
					{
						isSubLeftArmRotateFront = false;
					}
				}
				else
				{
					subLeftArmJoint--;
					if (subLeftArmJoint <= 0)
					{
						isSubLeftArmRotateFront = true;
					}
				}

			}
			else
			{
				leftArmJoint--;
				if (isSubLeftArmRotateFront == true)
				{
					subLeftArmJoint++;
					if (leftArmJoint >= 30)
					{
						isLeftArmRotateFront = false;
					}
					if (subLeftArmJoint >= 30)
					{
						isSubLeftArmRotateFront = false;
					}
				}
				else
				{
					if (leftArmJoint <= -30)
					{
						isLeftArmRotateFront = true;
					}
					if (subLeftArmJoint <= 0)
					{
						isSubLeftArmRotateFront = true;
					}
				}
			}

			if (isRightArmRotateFront == true)
			{
				rightArmJoint++;
				if (isSubRightArmRotateFront == true)
				{
					subRightArmJoint++;
					if (rightArmJoint >= 30)
					{
						isRightArmRotateFront = false;
					}
					if (subRightArmJoint >= 30)
					{
						isSubRightArmRotateFront = false;
					}
				}
				else
				{
					subRightArmJoint--;
					if (rightArmJoint >= 30)
					{
						isRightArmRotateFront = false;
					}
					if (subRightArmJoint <= 0)
					{
						isSubRightArmRotateFront = true;
					}
				}

			}
			else
			{
				rightArmJoint--;
				if (isSubRightArmRotateFront == true)
				{
					subRightArmJoint++;
					if (rightArmJoint >= 30)
					{
						isRightArmRotateFront = false;
					}
					if (subRightArmJoint >= 30)
					{
						isSubRightArmRotateFront = false;
					}
				}
				else
				{
					if (rightArmJoint <= -30)
					{
						isRightArmRotateFront = true;
					}
					if (subRightArmJoint <= 0)
					{
						isSubRightArmRotateFront = true;
					}
				}
			}

			if (isFingerClench == true)
			{
				firstFingerJoint++;
				secondFingerJoint++;
				thirdFingerJoint++;
				if (firstFingerJoint <= 0)
				{
					firstFingerJoint++;
				}
				if (firstFingerJoint >= maxFingerAngle)
				{
					isFingerClench = false;
				}
				if (secondFingerJoint <= 0)
				{
					secondFingerJoint++;
				}
				if (secondFingerJoint >= maxFingerAngle)
				{
					isFingerClench = false;
				}
				if (thirdFingerJoint <= 0)
				{
					thirdFingerJoint++;
				}
				if (thirdFingerJoint >= maxFingerAngle)
				{
					isFingerClench = false;
				}
			}
			else
			{
				firstFingerJoint--;
				secondFingerJoint--;
				thirdFingerJoint--;
				if (firstFingerJoint <= 0)
				{
					isFingerClench = true;
				}
				if (secondFingerJoint >= maxFingerAngle)
				{
					isFingerClench = true;
				}
				if (thirdFingerJoint >= maxFingerAngle)
				{
					isFingerClench = true;
				}


			}
		}
		else if (wParam == 0x53) //S key
		{
			if (isOrtho)
			{
				if (tz < orthoNear - 2.0)
				{
					tz += tSpeed;
				}
			}
			else
			{
				if (tz < perspecFar - s1Rad) // perspective far boundary
				{
					tz += tSpeed;
				}
			}
			if (isMaxLegAngle) //limits the leg turning angle to 30 and -30 degrees
			{
				rightLegAngle++;
				leftLegAngle--;

				if (rightLegAngle >= 30)
				{
					isMaxLegAngle = !isMaxLegAngle;
				}
			}
			else {
				rightLegAngle--;
				leftLegAngle++;

				if (rightLegAngle <= -30)
				{
					isMaxLegAngle = !isMaxLegAngle;
				}
			}

			if (!isDorsiFlexion) //limits the ankle rotation
			{
				if (rightAnkleAngle < rightAnkleDorsiFlexion) {
					rightAnkleAngle++;
				}
				else if (rightAnkleAngle == rightAnkleDorsiFlexion)
				{
					isDorsiFlexion = !isDorsiFlexion;
				}

				if (leftAnkleAngle > leftAnklePlantarFlexion)
				{
					leftAnkleAngle--;
				} //will auto change thanks to the if statement above

			}
			else { //dorsiflexion false
				if (rightAnkleAngle > rightAnklePlantarFlexion) {
					rightAnkleAngle--;
				}
				else if (rightAnkleAngle == rightAnklePlantarFlexion)
				{
					isDorsiFlexion = !isDorsiFlexion;
				}

				if (leftAnkleAngle < leftAnkleDorsiFlexion)
				{
					leftAnkleAngle++;
				} //will auto change thanks to the if statement above
			}

			if (isLeftArmRotateFront == true)
			{
				leftArmJoint++;
				if (isSubLeftArmRotateFront == true)
				{
					subLeftArmJoint++;
					if (leftArmJoint >= 30)
					{
						isLeftArmRotateFront = false;
					}
					if (subLeftArmJoint >= 30)
					{
						isSubLeftArmRotateFront = false;
					}
				}
				else
				{
					subLeftArmJoint--;
					if (subLeftArmJoint <= 0)
					{
						isSubLeftArmRotateFront = true;
					}
				}

			}
			else
			{
				leftArmJoint--;
				if (isSubLeftArmRotateFront == true)
				{
					subLeftArmJoint++;
					if (leftArmJoint >= 30)
					{
						isLeftArmRotateFront = false;
					}
					if (subLeftArmJoint >= 30)
					{
						isSubLeftArmRotateFront = false;
					}
				}
				else
				{
					if (leftArmJoint <= -30)
					{
						isLeftArmRotateFront = true;
					}
					if (subLeftArmJoint <= 0)
					{
						isSubLeftArmRotateFront = true;
					}
				}
			}

			if (isRightArmRotateFront == true)
			{
				rightArmJoint++;
				if (isSubRightArmRotateFront == true)
				{
					subRightArmJoint++;
					if (rightArmJoint >= 30)
					{
						isRightArmRotateFront = false;
					}
					if (subRightArmJoint >= 30)
					{
						isSubRightArmRotateFront = false;
					}
				}
				else
				{
					subRightArmJoint--;
					if (rightArmJoint >= 30)
					{
						isRightArmRotateFront = false;
					}
					if (subRightArmJoint <= 0)
					{
						isSubRightArmRotateFront = true;
					}
				}

			}
			else
			{
				rightArmJoint--;
				if (isSubRightArmRotateFront == true)
				{
					subRightArmJoint++;
					if (rightArmJoint >= 30)
					{
						isRightArmRotateFront = false;
					}
					if (subRightArmJoint >= 30)
					{
						isSubRightArmRotateFront = false;
					}
				}
				else
				{
					if (rightArmJoint <= -30)
					{
						isRightArmRotateFront = true;
					}
					if (subRightArmJoint <= 0)
					{
						isSubRightArmRotateFront = true;
					}
				}
			}
		}
		else if (wParam == VK_UP)
		{
			viewAngleY += 3;
		}
		else if (wParam == VK_DOWN)
		{
			viewAngleY -= 3;
		}
		else if (wParam == VK_LEFT)
		{
			viewAngleX += 3;
		}
		else if (wParam == VK_RIGHT)
			viewAngleX -= 3;
		else if (wParam == VK_SPACE) //reset
		{
			rightLegAngle = 0;
			leftLegAngle = 0;
			leftAnkleAngle = 0;
			rightAnkleAngle = 0;
			viewAngleX = 0;
			viewAngleY = 0;

		}
		else if (wParam == 0x30)	//zero number
		{
			isWeapon = !isWeapon;
		}
		else if (wParam == 0x38)		// number 1
		{
			if (isWeapon)
			{
				weaponActivated = !weaponActivated;
				attackPhase = false;
			}
			else
			{
				weaponActivated = false;
			}
		}
		else if (wParam == 0x39)
		{


			if (weaponActivated)
			{
				attackPhase = true;
				if (swordOut) //holding sword down
				{
					rotateShoulderToSwingXAxis = 20;
				}
				else
				{
					rotateShoulderToSwingXAxis = 60;
				}
				swordOut = !swordOut;
			}
		}

		//Projection
		else if (wParam == 0x31) //1 key
		{
			isLightOn = !isLightOn;
		}

		else if (wParam == 0x58) // X
		{
			if (isOrtho)
			{
				isOrtho = false;
				tz = perspecNear + s1Rad;

			}
			else
			{
				isOrtho = true;
				tx = 0.0f;
			}
			tz = 0;
		}
		else if (wParam == 'A')
		{
			ptx -= ptSpeed;		// move projection to left
		}
		else if (wParam == 'D')
		{
			ptx += ptSpeed;		// move to right
		}
		else if (wParam == 'T')
		{
			pty += ptSpeed;		// Move projection to up
		}
		else if (wParam == 'G')
		{
			pty -= ptSpeed;		// Move projection to down
		}
		//Light
		else if (wParam == 'I') //I key
		{
			posA[1] += 1;
		}
		else if (wParam == 'K') //K key
		{
			posA[1] -= 1;
		}
		else if (wParam == 'J') //J Key
		{
			posA[0] -= 10;
		}
		else if (wParam == 'L') //L Key
		{
			posA[0] += 10;
		}
		else if (wParam == 'N') //N Key
		{
			posA[2] += 10;
		}
		else if (wParam == 0x32) {
			canloadTexture = !canloadTexture;
		}
		else if (wParam == 'M') posA[2] -= 10;
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

void drawDisk(float inr, float outr)
{
	GLUquadricObj* disk = NULL; //declare quadric obj pointer
	disk = gluNewQuadric(); // create quadric obj in the memory
	gluQuadricDrawStyle(disk, GLU_FILL); //draw using line
	gluDisk(disk, inr, outr, 30, 30); //draw using disk
	gluDeleteQuadric(disk);	//destroy the obj & free up memory
	//one glu command is one polygon
}

void drawSphere(float r)
{
	GLUquadricObj* sphere = NULL; //declare quadric obj pointer
	sphere = gluNewQuadric(); // create quadric obj in the memory
	gluQuadricDrawStyle(sphere, GLU_FILL); //draw using line
	gluSphere(sphere, r, 30, 30); //draw using sphere
	gluDeleteQuadric(sphere);	//destroy the obj & free up memory
	//one glu command is one polygon
}

void drawRec(float width, float length, float depth)
{
	glBegin(GL_QUADS);
	//back quad
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);
	glVertex3f(width, length, 0);
	glVertex3f(width, 0, 0);

	//back quad
	glVertex3f(0, 0, depth);
	glVertex3f(0, length, depth);
	glVertex3f(width, length, depth);
	glVertex3f(width, 0, depth);

	//left quad
	glVertex3f(0, 0, 0);
	glVertex3f(0, length, 0);
	glVertex3f(0, length, depth);
	glVertex3f(0, 0, depth);

	//right quad
	glVertex3f(width, 0, 0);
	glVertex3f(width, length, 0);
	glVertex3f(width, length, depth);
	glVertex3f(width, 0, depth);

	//bottom quad
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, depth);
	glVertex3f(width, 0, depth);
	glVertex3f(width, 0, 0);

	//top quad
	glVertex3f(0, length, 0);
	glVertex3f(0, length, depth);
	glVertex3f(width, length, depth);
	glVertex3f(width, length, 0);
	glEnd();
}

void drawThigh(float width, float length, float depth)
{
	float upperThighLength = 0.05;

	glBegin(GL_QUADS);
	//front quad
	glVertex3f(0 - upperThighLength, 0, 0 + upperThighLength);
	glVertex3f(0, length, 0);
	glVertex3f(width, length, 0);
	glVertex3f(width + upperThighLength, 0, 0 + upperThighLength);

	//back quad
	glVertex3f(0 - upperThighLength, 0, depth - upperThighLength);
	glVertex3f(0, length, depth);
	glVertex3f(width, length, depth);
	glVertex3f(width + upperThighLength, 0, depth - upperThighLength);

	//left quad
	glVertex3f(0 - upperThighLength, 0, 0 + upperThighLength);
	glVertex3f(0, length, 0);
	glVertex3f(0, length, depth);
	glVertex3f(0 - upperThighLength, 0, depth - upperThighLength);

	//right quad
	glVertex3f(width + upperThighLength, 0, 0 + upperThighLength);
	glVertex3f(width, length, 0);
	glVertex3f(width, length, depth);
	glVertex3f(width + upperThighLength, 0, depth - upperThighLength);

	//top quad
	glVertex3f(0 - upperThighLength, 0, 0);
	glVertex3f(0 - upperThighLength, 0, depth - upperThighLength);
	glVertex3f(width + upperThighLength, 0, depth - upperThighLength);
	glVertex3f(width + upperThighLength, 0, 0);

	//bottom quad
	glVertex3f(0, length, 0);
	glVertex3f(0, length, depth);
	glVertex3f(width, length, depth);
	glVertex3f(width, length, 0);
	glEnd();
} //used for cube thighs

void drawCylinder(float br, float tr, float h)
{
	GLUquadricObj* cylinder = NULL; //declare quadric obj pointer
	cylinder = gluNewQuadric(); // create quadric obj in the memory
	gluQuadricDrawStyle(cylinder, GLU_FILL); //draw using line
	gluCylinder(cylinder, br, tr, h, 30, 30); //draw using cylinder
	gluDeleteQuadric(cylinder);	//destroy the obj & free up memory
	//one glu command is one polygon
}

void drawCube(float size)
{
	float halfSize = size;

	glBegin(GL_QUADS);
	//Face 1: FRONT
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-halfSize, -halfSize, -halfSize);

	//Face 2: LEFT
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-halfSize, -halfSize, halfSize);

	//Face 3: Top
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(halfSize, -halfSize, halfSize);

	//Face 4: RIGHT
	glTexCoord2f(0.0, 1.0);
	glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(halfSize, -halfSize, -halfSize);

	//Face 5: BOTTOM
	glTexCoord2f(0.0, 1.0);
	glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(halfSize, halfSize, -halfSize);

	//FACE 6: BACK
	glTexCoord2f(0.0, 1.0);
	glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(halfSize, halfSize, halfSize);

	glEnd();
}

//LEGS DETAILS
void drawLegFan()
{
	int noOfFan = 3;
	float fanMidx = 0, fanMidy = 0, fanMidz = 0, fanRadius = 0.1;
	float fanX = -0.03, fanY = 0, fanZ = -0.1;

	glPushMatrix();
	glRotatef(legFanAngle, 0, 0, 1);
	for (float angle = PI / 4; angle <= (2 * PI); angle += (2 * PI) / noOfFan) //2 * PI = 360 degrees.
	{
		glBegin(GL_TRIANGLES); //windmill fan
		glColor3f(1, 1, 1);
		glVertex3f(fanMidx, fanMidy, fanMidz); //mid point
		fanX = fanMidx + fanRadius * cos(angle);
		fanY = fanMidy + fanRadius * sin(angle);
		glVertex3f(fanX, fanY, fanZ);

		fanX = fanMidx + fanRadius * cos(angle + PI / 9); //45 + 20 = 50 degrees
		fanY = fanMidy + fanRadius * sin(angle + PI / 9);
		glVertex3f(fanX, fanY, fanZ);
		glEnd();
	}

	legFanAngle = legFanAngle + 0.5;

	glPopMatrix();
}

void drawLegWheel()
{
	glPushMatrix();
	glColor3f(0.5, 0.5, 0.5);

	glPushMatrix();
	glTranslatef(0, 0, -0.1);
	drawCylinder(0.1, 0.08, 0.1);
	glPopMatrix();

	drawLegFan();

	glColor3f(1, 0, 0);
	glTranslatef(0, 0, -0.05);
	drawSphere(0.05);

	glPopMatrix();
}

void drawThighArmor()
{
	glPushMatrix();
	//WHEEL
	glPushMatrix();
	glTranslatef(-0.03, 0, -0.05);
	glRotatef(90, 0, 1, 0);
	glRotatef(-25, 1, 0, 0);
	drawLegWheel();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.01, 0, 0);
	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_QUADS);
	//left quad -z is torwards us
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.15, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.15, -0.125);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.1, -0.15);


	//front quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, -0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.15, -0.125);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.05, -0.15, -0.125);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.1, -0.15);


	//back quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.15, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.05, -0.15, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.1, 0.05);


	//bottom quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.05, -0.15, -0.125);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.15, -0.125);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.15, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.05, -0.15, 0.025);


	//top quad
	glColor3f(0.3, 0.3, 0.3);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, -0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0.1, -0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.1, 0.05);

	glEnd();
	glPopMatrix();
	glTranslatef(0.075, -0.3, -0.05); //TO GET MIDPOINT OF THIGH
	glTranslatef(0, 0, -0.1);

	glBegin(GL_QUADS);
	//front thigh cover
	//left part
	glColor3f(0.3, 0.3, 0.3);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0.125, 0); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0.3, -0.05); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.25, 0); // Top left

	//right part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.125, 0); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.1, 0); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.25, 0); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.3, -0.05); // Top left

	//left thigh cover
	//left part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0.2); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.125, 0.1); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.15, 0.3, 0.1); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.25, 0.2); // Top left

	//right part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.125, 0.1); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, 0); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1, 0.25, 0); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.15, 0.3, 0.1); // Top left

	//right thigh cover
	//left part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.1, 0.2); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.125, 0.1); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.15, 0.3, 0.1); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.25, 0.2); // Top left

	//right part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.125, 0.1); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.1, 0); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.25, 0); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.15, 0.3, 0.1); // Top left

	//back thigh cover
	//left part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0.2); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0.125, 0.2); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0.3, 0.25); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.25, 0.2); // Top left

	//right part
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.125, 0.2); // Left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.1, 0.2); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.25, 0.2); // Top right

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.3, 0.25); // Top left



	glColor3f(0, 0, 0);
	//SHARP THING
	//front left corner
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.125, 0.1); // Left quad top

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, 0); // Bottom

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0.125, 0); // Right quad top

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.125, 0.5, -0.1); // Pointy top

	//front right corner
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.125, 0.1); // Left quad top

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.1, 0); // Bottom

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0.125, 0); // Right quad top

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.125, 0.5, -0.1); // Pointy top

	//back left corner
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.125, 0.2); // Left quad top from back knee quad

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, 0.2); // Bottom

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1, 0.125, 0.1); // Right quad top from left knee quad

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.125, 0.5, 0.3); // Pointy top

	//back right corner
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.125, 0.2); // Left quad top from back knee quad

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.1, 0.2); // Bottom

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.125, 0.1); // Right quad top from left knee quad

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.125, 0.5, 0.3); // Pointy top
	glEnd();
	glPopMatrix();
}

void drawKneeArmor()
{
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0, 0, -0.1);

	glPushMatrix();
	glTranslatef(-0.05, 0, 0.1);
	glRotatef(90, 0, 1, 0);
	glScalef(0.75, 0.75, 0.75);
	//drawLegWheel();
	glPopMatrix();

	glBegin(GL_QUADS);
	//front quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.125, 0);   // Top

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, 0);  // Left

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.125, 0);  // Bottom

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.1, 0);   // Right

	//left quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.125, 0.1);   // Top

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, 0.2);    // Left

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1); // Bottom

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.1, 0);       // Right

	//right quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.125, 0.1);    // Top

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1, 0.1, 0.2);      // Left

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1);   // Bottom

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.1, 0);        // Right

	//back quad
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.125, 0.2);   // Top

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.1, 0.2);  // Left

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.125, 0.2);  // Bottom

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.1, 0.2);   // Right
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.3, 0.3, 0.3);


	//front right triangle
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.1, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.125, 0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1); // Bottom


	//front left triangle
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.125, 0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1); // Bottom


	//back right triangle
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.1, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.125, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1); // Bottom


	//back left triangle
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.1, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.125, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1); // Bottom


	//front bottom right triangle
	glColor3f(0, 0, 0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.125, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1); // Right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.225, -0.01); // Front

	//front bottom left triangle
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.125, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1); // Left

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.225, -0.01); // Front

	//back bottom right triangle
	glColor3f(0, 0, 0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.125, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.225, 0.21); // Front


	//back bottom left triangle
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.125, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.225, 0.21); // Front


	//shinguard front bottom left tri
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.225, -0.01); // Front

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.45, 0.05);


	//shinguard front bottom right tri
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.225, -0.01); // Front

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.45, 0.05);

	//shinguard back bottom left tri
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.225, 0.21); // Front

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.125, -0.2, 0.1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.45, 0.16);


	//shinguard back bottom right tri
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.225, 0.21); // Front

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.125, -0.2, 0.1);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.45, 0.16);


	//shinguard left tri
	glColor3f(0.2, 0.2, 0.2);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.125, -0.2, 0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.45, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.45, 0.16);


	//shinguard right tri
	glColor3f(0.2, 0.2, 0.2);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.125, -0.2, 0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.45, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.45, 0.16);
	glEnd();
	glPopMatrix();
}

void drawAnkleArmor()
{
	glPushMatrix();
	glTranslatef(-0.05, 0, 0);
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	drawLegWheel();
	glPopMatrix();

	glTranslatef(-0.02, -0.05, 0);
	glBegin(GL_QUADS);
	glColor3f(0.3, 0.3, 0.3);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.1, -0.03); // Top left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, -0.05);   // Bottom left

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.1, 0.03);  // Top right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, 0, 0.05);    // Bottom right
	glEnd();

	glPopMatrix();
}

void drawLeftFeetArmor()
{
	glPushMatrix();
	glTranslatef(0.01, 0.175, -0.1);
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.3, 0.3, 0.3);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0, -0.05);      // Left part z----->
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, 0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.02, -0.1, -0.05);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.02, -0.1, 0.1);

	glTexCoord2f(2.0, 1.0);
	glVertex3f(-0.02, -0.175, -0.05);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-0.02, -0.175, 0.1);

	glEnd();

	glBegin(GL_QUAD_STRIP); //middle part
	glColor3f(0, 0, 0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0, -0.05);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.05, -0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.02, -0.1, -0.05);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.02, -0.1, -0.15);

	glTexCoord2f(2.0, 1.0);
	glVertex3f(-0.02, -0.175, -0.05);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-0.02, -0.175, -0.15);
	glEnd();

	glBegin(GL_QUAD_STRIP); //front part
	glColor3f(0.5, 0.5, 0.5);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.05, -0.15);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.02, -0.1, -0.20);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.02, -0.1, -0.15);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.02, -0.1, -0.20);

	glTexCoord2f(2.0, 1.0);
	glVertex3f(-0.02, -0.175, -0.15);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(0.02, -0.175, -0.20);
	glEnd();
	glPopMatrix();
}

void drawFeetArmor()
{
	glPushMatrix(); //width is 0.13 long
	drawLeftFeetArmor();
	glTranslatef(0.15, 0, 0);
	glScalef(-1, 1, 1);
	drawLeftFeetArmor();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.01, 0.175, -0.1);
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.3, 0.3, 0.3);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.13, 0, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.13, 0, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0, 0.05);

	glColor3f(0, 0, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.13, 0, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.13, -0.05, -0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.05, -0.15);

	//front partt
	glColor3f(0.3, 0.3, 0.3);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.05, -0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.13, -0.05, -0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, -0.1, -0.20);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.02, -0.1, -0.20);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	//back cover
	glColor3f(0.3, 0.3, 0.2);
	glVertex3f(0, 0, 0.05); //top left 
	glVertex3f(0.13, 0, 0.05); //top right
	glVertex3f(-0.02, -0.1, 0.1);
	glVertex3f(0.15, -0.1, 0.1);
	glVertex3f(-0.02, -0.175, 0.1);
	glVertex3f(0.15, -0.175, 0.1);

	//bottom cover
	glColor3f(0, 0, 0);
	glVertex3f(-0.02, -0.175, -0.15);
	glVertex3f(0.15, -0.175, -0.15);

	glVertex3f(0.02, -0.175, -0.2);
	glVertex3f(0.11, -0.175, -0.2);
	glEnd();

	glBegin(GL_QUADS); //front cover
	glColor3f(0.3, 0.3, 0.3);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.02, -0.1, -0.2); // Top left

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.02, -0.175, -0.20); // Bottom left

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.11, -0.1, -0.2); // Top right

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, -0.175, -0.20); // Bottom right
	glEnd();
	glPopMatrix();
}

//DRAW LEGS
void drawKnees()
{
	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslatef(0.075, -0.3, -0.05); //TO GET MIDPOINT OF THIGH
	drawSphere(0.075);
	drawKneeArmor();
	glPopMatrix();
}

void drawAnkle()
{
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(0.075, -0.67, -0.05);
	drawSphere(0.060);
	drawAnkleArmor();

	glScalef(-1, 1, 1);
	drawAnkleArmor();
	glPopMatrix();
}

void drawRightFeet()
{
	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(0, -0.875, 0.05);
	glRotatef(rightAnkleAngle, 1, 0, 0);
	//drawRec(0.15, 0.10, -0.25);
	drawFeetArmor();
	glPopMatrix();
}

void drawLeftFeet()
{
	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(0, -0.875, 0.05);
	glRotatef(leftAnkleAngle, 1, 0, 0);
	//drawRec(0.15, 0.10, -0.25);
	drawFeetArmor();
	glPopMatrix();
}

void drawCalf()
{
	glPushMatrix();
	//glTranslatef(0.025, -0.35, -0.015); //thigh - calve divide by 2
	glColor3f(1, 0, 0);
	glTranslatef(0.075, -0.34, -0.055);
	glRotatef(90, 1, 0, 0);
	drawCylinder(0.070, 0.08, 0.15);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.075, -0.64, -0.055); //-0.59 = first translate (-0.29) + height of both cylinders
	glRotatef(270, 1, 0, 0);
	glColor3f(1, 1, 0);
	drawCylinder(0.060, 0.08, 0.15);
	//drawRec(0.1, -0.25, -0.07);
	glPopMatrix();
}

void drawHips()
{
	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslatef(0.075, 0.1, -0.05); //TO GET MIDPOINT OF THIGH
	drawSphere(0.125);
	glPopMatrix();
}

void drawRightLeg()
{
	glPushMatrix();
	glPushMatrix();
	glScalef(-1, 1, 1);
	glTranslatef(-0.15, 0, 0);
	drawThighArmor();
	glPopMatrix();
	drawHips();
	glColor3f(0, 1, 0);
	glTranslatef(0.075, 0.05, -0.050); //idk why its 0.050
	glRotatef(90, 1, 0, 0);
	drawCylinder(0.10, 0.075, 0.3);
	glPopMatrix();
	//drawThigh(0.15, -0.25, -0.10);
	glTranslatef(0.15, 0, 0);
	glScalef(-1, 1, 1);
	drawKnees();

	glPushMatrix();
	//glRotatef(rightLegAngle - kneeAngle, 1, 0, 0);
	drawCalf();
	drawAnkle();
	drawRightFeet();
	glPopMatrix();
}

void drawLeftLeg()
{
	glPushMatrix();
	drawThighArmor();
	drawHips();
	glColor3f(0, 1, 0);
	glTranslatef(0.075, 0.05, -0.050); //idk why its 0.050
	glRotatef(90, 1, 0, 0);
	drawCylinder(0.1, 0.075, 0.3);
	glPopMatrix();
	//drawThigh(0.15, -0.25, -0.10);
	drawKnees();

	glPushMatrix();
	drawCalf();
	glPopMatrix();

	drawAnkle();
	drawLeftFeet();
}

void drawBothLegs()
{
	glPushMatrix();
	glTranslatef(0, -0.05, 0.05);
	glPushMatrix();
	glTranslatef(-0.2, 0, 0); //0.05 + width of thigh
	glRotatef(leftLegAngle, 1, 0, 0);
	drawLeftLeg();
	glPopMatrix();
	glPushMatrix();
	//right leg
	glTranslatef(0.05, 0, 0);
	glRotatef(rightLegAngle, 1, 0, 0);
	drawRightLeg();
	glPopMatrix();
	glPopMatrix();
}


//Body Details 
void drawNeckArmor(float x) {

	//Back Neck Armor 
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(0, 0.72, -0.236);
	glBegin(GL_POLYGON);
	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.025), 0.2, 0.4);
	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.025), 0.4, 0.45);
	glTexCoord2f(0.5, 0.5);
	glVertex3f(0, 0.3, 0.45);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.025), 0.4, 0.45);
	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.025), 0.2, 0.4);
	glEnd();
	glPopMatrix();

	//Left Neck Armor 1
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(-0.15, 0.72, -0.236);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.225, 0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(x, 0.1, -0.05);

	glTexCoord2f(1.0, 0.5);
	glVertex3f(x, 0.15, -0.05);

	glTexCoord2f(0.5, 0.0);
	glVertex3f((-x + 0.125), 0.275, 0);

	glTexCoord2f(0.0, 0.5);
	glVertex3f((-x + 0.125), 0.225, 0.05);
	glEnd();

	glPopMatrix();

	//Left Neck Armor 2
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(-0.15, 0.72, -0.236);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.125), 0.275, 0.);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.125), 0.225, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.1), 0.225, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.1), 0.3, 0.15);
	glEnd();

	glPopMatrix();

	//Left Neck Armor 3
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(-0.15, 0.72, -0.236);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.1), 0.3, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.1), 0.225, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.125), 0.2, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.125), 0.4, 0.45);
	glEnd();

	glPopMatrix();

	//Right Neck Armor 1
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(0.15, 0.72, -0.236);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.225, 0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-x, 0.1, -0.05);

	glTexCoord2f(1.0, 0.5);
	glVertex3f(-x, 0.15, -0.05);

	glTexCoord2f(0.5, 0.0);
	glVertex3f((x - 0.125), 0.275, 0);

	glTexCoord2f(0.0, 0.5);
	glVertex3f((x - 0.125), 0.225, 0.05);
	glEnd();

	glPopMatrix();

	//Right Neck Armor 2
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(0.15, 0.72, -0.236);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.125), 0.275, 0.);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.125), 0.225, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.1), 0.225, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.1), 0.3, 0.15);
	glEnd();

	glPopMatrix();

	//Left Neck Armor 3
	glColor3f(0.85, 0, 0.4);
	glPushMatrix();
	glTranslatef(0.15, 0.72, -0.236);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.1), 0.3, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.1), 0.225, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.125), 0.2, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.125), 0.4, 0.45);
	glEnd();

	glPopMatrix();

}

void drawChestArmor(float x) {


	//LeftChest
	glColor3f(1, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.72, -0.236);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(0.0, 0.15, -0.1);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(x, 0.1, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(x, -0.1, -0.1);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, -0.15, -0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-x, -0.1, 0.05);

	glTexCoord2f(0.0, 0.5);
	glVertex3f((-x + 0.025), 0.1, 0.05);
	glEnd();

	glPopMatrix();

	//Triangle above armor ()2 tirangles
	/*glColor3f(1, 0, 1);
	glBegin(GL_TRIANGLES);
		glVertex3f(0, 0.15, 0);
		glVertex3f(-x, 0.1, 0);
		glVertex3f(0, 0.2, 0);
		glVertex3f(0, 0.15, 0);
		glVertex3f(x, 0.1, 0);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3f(0, 0.15, 0);
		glVertex3f(x, 0.1, 0);
		glVertex3f(0, 0.2, 0);
		glVertex3f(0, 0.15, 0);
	glEnd();
	glPopMatrix();*/

	//Left Triangle above armor (1 tirangle strip
	glColor3f(1, 0, 1);
	glPushMatrix();
	glTranslatef(-0.15, 0.72, -0.236);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(0, 0.15, -0.1);
	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.025), 0.1, 0.05);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, 0.225, 0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(x, 0.1, -0.05);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(0, 0.15, -0.1);
	glEnd();
	glPopMatrix();


	//Left Chest Armor 2
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.025), 0.4, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-x, 0.2, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.025), 0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.025), 0.4, 0.15);
	glEnd();
	glPopMatrix();


	//Left Chest Armor 3
	glColor3f(0.5, 0.3, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.025), 0.4, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.025), 0.2, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.025), 0.2, 0.3);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.025), 0.4, 0.3);
	glEnd();

	glPopMatrix();

	//Left Chest Armor 4
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.025), 0.4, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.025), 0.2, 0.3);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.025), 0.2, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.025), 0.4, 0.35);
	glEnd();
	glPopMatrix();


	//RightChest
	glColor3f(1, 0, 0.2);

	glPushMatrix();
	glTranslatef(0.15, 0.72, -0.236);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(0.0, 0.15, -0.1);
	glTexCoord2f(0.0, 0.5);
	glVertex3f((x - 0.025), 0.1, 0.05);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(x, -0.1, 0.05);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, -0.15, -0.1);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-x, -0.1, -0.1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-x, 0.1, -0.05);

	glEnd();



	glPopMatrix();

	//Right Triangle above armor (1 tirangle strip
	glColor3f(1, 0, 1);
	glPushMatrix();
	glTranslatef(0.15, 0.72, -0.236);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(0, 0.15, -0.1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.025), 0.1, 0.05);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, 0.225, 0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-x, 0.1, -0.05);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(0, 0.15, -0.1);
	glEnd();
	glPopMatrix();



	//Right Chest Armor 2
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.025), 0.4, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(x, 0.2, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.025), 0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.025), 0.4, 0.15);
	glEnd();
	glPopMatrix();


	//Right Chest Armor 3
	glColor3f(0.5, 0.3, 0.2);
	glPushMatrix();
	glTranslatef(0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.025), 0.4, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.025), 0.2, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.025), 0.2, 0.3);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.025), 0.4, 0.3);
	glEnd();
	glPopMatrix();

	//Right Chest Armor 4
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.025), 0.4, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.025), 0.2, 0.3);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.025), 0.2, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.025), 0.4, 0.35);
	glEnd();
	glPopMatrix();

	//Middle hexagon
	glColor3f(0.5, 0, 0);

	glPushMatrix();
	glTranslatef(0.0, 0.5, -0.337);

	glBegin(GL_POLYGON);
	glVertex3f(0.0, 0.15, -0.0);
	glVertex3f(x, 0.075, 0);
	glVertex3f((x - 0.05), -0.075, 0);
	glVertex3f(0, -0.15, -0.0);
	glVertex3f((-x + 0.05), -0.075, 0);
	glVertex3f(-x, 0.075, 0);
	glVertex3f(0.0, 0.15, -0.0);
	glEnd();

	glPopMatrix();

	//Middle hexagon 2
	glColor3f(0.6, 0.2, 0.35);

	glPushMatrix();
	glTranslatef(0.0, 0.5, -0.336);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(0, 0.075, 0.0);

	glTexCoord2f(0.8, 1.0);
	glVertex3f(x, 0.075, 0);

	glTexCoord2f(0.0, 0.8);
	glVertex3f((x + 0.025), 0.05, 0);

	glTexCoord2f(0.0, 0.2);
	glVertex3f((x - 0.025), -0.075, 0);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, -0.175, 0.0);

	glTexCoord2f(1.0, 0.8);
	glVertex3f((-x + 0.025), -0.075, 0);

	glTexCoord2f(1.0, 0.2);
	glVertex3f((-x - 0.025), 0.05, 0);

	glTexCoord2f(0.2, 1.0);
	glVertex3f(-x, 0.075, 0);

	glTexCoord2f(0.5, 1.0);
	glVertex3f(0, 0.075, 0.0);
	glEnd();
	glPopMatrix();


	drawNeckArmor(x);

}

void drawWaistArmorDown(float x) {

	//Middle Waist Front Armor 
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02), -0.1, -0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02), -0.2, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02), -0.2, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02), -0.1, -0.05);
	glEnd();

	glPopMatrix();


	//Middle Waist Back Armor Down
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.15, -0.1, 0.35);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, -0.2, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.155, -0.2, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.155, -0.1, 0.35);
	glEnd();

	glPopMatrix();

	//Left Waist Armor 1 Down
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.1, -0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.2, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.1), -0.2, 0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.1), -0.1, 0.05);

	glEnd();
	glPopMatrix();


	//Left Waist Armor 2 Down
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.1), -0.1, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.1), -0.2, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.075), -0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.075), -0.1, 0.15);
	glEnd();

	glPopMatrix();

	//Left Waist Armor 3 Down
	glColor3f(0.5, 0.3, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.075), -0.1, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.075), -0.2, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.1 - 0.025), -0.2, 0.3);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.1 - 0.025), -0.1, 0.3);
	glEnd();

	glPopMatrix();

	//Left Waist Armor 4 Down
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.1 - 0.025), -0.1, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.1 - 0.025), -0.2, 0.3);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.125), -0.2, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.125), -0.1, 0.35);
	glEnd();
	glPopMatrix();



	//Right Waist Armor 1 Down
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, -0.1, -0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.2, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.1), -0.2, 0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.1), -0.1, 0.05);
	glEnd();

	glPopMatrix();


	//Right Waist Armor 2 Down
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.1), -0.1, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.1), -0.2, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.075), -0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.075), -0.1, 0.15);
	glEnd();

	glPopMatrix();

	//Right Waist Armor 3 Down
	glColor3f(0.5, 0.3, 0.2);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.075), -0.1, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.075), -0.2, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.1 + 0.025), -0.2, 0.3);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.1 + 0.025), -0.1, 0.3);
	glEnd();

	glPopMatrix();

	//Right Waist Armor 4 Down
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.1 + 0.025), -0.1, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.1 + 0.025), -0.2, 0.3);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.125), -0.2, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.125), -0.1, 0.35);
	glEnd();


	glPopMatrix();



}

void drawWaistArmor(float x) {

	//Middle Waist Front Armor 
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02), 0.15, -0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02), -0.1, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02), -0.1, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02), 0.15, -0.1);
	glEnd();
	glPopMatrix();

	//Middle Waist Back Armor 
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.15, 0.2, 0.4);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, -0.1, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.155, -0.1, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.155, 0.2, 0.4);
	glEnd();

	glPopMatrix();

	//Middle Chest Back Armor 
	glColor3f(0.4, 0.1, 0.2);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	//Middle up
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.15, 0.2, 0.4);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, 0.4, 0.4);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.155, 0.4, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.155, 0.2, 0.4);
	glEnd();




	//Left
	glColor3f(0.4, 0.2, 0.4);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.025 - 0.15), 0.4, 0.35);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.025 - 0.15), 0.2, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.15, 0.2, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.15, 0.4, 0.4);

	glEnd();


	//Right
	glColor3f(0.4, 0.2, 0.4);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.025 + 0.15), 0.4, 0.35);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.025 + 0.15), 0.2, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.15, 0.2, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.15, 0.4, 0.4);
	glEnd();


	glPopMatrix();


	//Left Waist Armor 5
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.025), 0.2, 0.35);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.125), -0.1, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.1, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.2, 0.4);

	glEnd();

	glPopMatrix();



	//Left Waist Armor 1
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.2, -0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.1, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.1), -0.1, 0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-x, 0.2, 0.05);

	glEnd();

	glPopMatrix();


	//Left Waist Armor 2
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-x, 0.2, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.1), -0.1, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.075), -0.1, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.025), 0.2, 0.15);

	glEnd();

	glPopMatrix();

	//Left Waist Armor 3
	glColor3f(0.5, 0.3, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.025), 0.2, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.075), -0.1, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.1 - 0.025), -0.1, 0.3);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.025), 0.2, 0.3);

	glEnd();

	glPopMatrix();

	//Left Waist Armor 4
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.025), 0.2, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.1 - 0.025), -0.1, 0.3);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x + 0.125), -0.1, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x + 0.025), 0.2, 0.35);

	glEnd();

	glPopMatrix();

	//Left Waist Armor 5
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(-0.15, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x + 0.025), 0.2, 0.35);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x + 0.125), -0.1, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.1, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.2, 0.4);

	glEnd();

	glPopMatrix();

	//Right Waist Armor 1
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.15, -0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, -0.1, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.1), -0.1, 0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(x, 0.2, 0.05);

	glEnd();

	glPopMatrix();

	//Right Waist Armor 2
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(x, 0.2, 0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.1), -0.1, 0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.075), -0.1, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.025), 0.2, 0.15);
	glEnd();

	glPopMatrix();

	//Right Waist Armor 3
	glColor3f(0.5, 0.3, 0.2);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.025), 0.2, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.075), -0.1, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.1 + 0.025), -0.1, 0.3);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.025), 0.2, 0.3);

	glEnd();

	glPopMatrix();

	//Right Waist Armor 4
	glColor3f(0.5, 0, 0.2);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.025), 0.2, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.1 + 0.025), -0.1, 0.3);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x - 0.125), -0.1, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x - 0.025), 0.2, 0.35);

	glEnd();

	glPopMatrix();

	//Right Waist Armor 5
	glColor3f(0.5, 0, 0.5);
	glPushMatrix();
	glTranslatef(0.155, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.025), 0.2, 0.35);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.125), -0.1, 0.35);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0, -0.1, 0.35);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.2, 0.4);

	glEnd();

	glPopMatrix();


	drawWaistArmorDown(x);

}

void drawPiguArmor(float x) {

	//Middle Pigu Armor 
	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02), -0.2, -0.05);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02), -0.4, -0.05);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02), -0.4, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02), -0.2, -0.05);

	glEnd();

	glPopMatrix();


	//Middle Pigu Front Armor 
	glColor3f(0.9, 0.9, 0.9);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.5, 0.7);
	glVertex3f(0, -0.3, -0.1);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.075), -0.1, -0.0);
	glTexCoord2f(1.0, 0.3);
	glVertex3f((x + 0.02 + 0.05), -0.4, -0.075);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, -0.5, -0.1);

	glTexCoord2f(0.0, 0.3);
	glVertex3f((-x - 0.02 - 0.05), -0.4, -0.075);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.075), -0.1, -0.0);

	glTexCoord2f(0.5, 0.7);
	glVertex3f(0, -0.3, -0.1);
	glEnd();
	glPopMatrix();


	//Middle Pigu Back Armor 
	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.5, 0.7);
	glVertex3f(0, -0.2, 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.075), -0.1, 0.4);
	glTexCoord2f(1.0, 0.3);
	glVertex3f((x + 0.02 + 0.05), -0.4, 0.475);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0, -0.5, 0.5);

	glTexCoord2f(0.0, 0.3);
	glVertex3f((-x - 0.02 - 0.05), -0.4, 0.475);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.075), -0.1, 0.4);
	glTexCoord2f(0.5, 0.7);
	glVertex3f(0, -0.2, 0.5);
	glEnd();
	glPopMatrix();




	//Middle Pigu Left Armor 1
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.075), -0.1, -0.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.02 - 0.05), -0.4, -0.075);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02 - 0.2), -0.5, -0.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.075);

	glEnd();

	glPopMatrix();

	//Middle Pigu Left Armor 2
	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.075);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.02 - 0.2), -0.5, 0.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02 - 0.15), -0.3, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.1);

	glEnd();

	glPopMatrix();

	//Middle Pigu Left Armor 3
	glColor3f(0.7, 0.95, 0.7);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.02 - 0.15), -0.3, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02 - 0.15), -0.3, 0.25);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.3);

	glEnd();

	glPopMatrix();

	//Middle Pigu Left Armor 4
	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.02 - 0.15), -0.3, 0.25);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02 - 0.2), -0.5, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.325);

	glEnd();

	glPopMatrix();

	//Middle Pigu Left Armor 5
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((-x - 0.02 - 0.1), -0.1, 0.325);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((-x - 0.02 - 0.2), -0.5, 0.4);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((-x - 0.02 - 0.05), -0.4, 0.475);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((-x - 0.02 - 0.075), -0.1, 0.4);

	glEnd();

	glPopMatrix();



	//Middle Pigu Right Armor 1
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02 + 0.075), -0.1, -0.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02 + 0.05), -0.4, -0.075);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.02 + 0.2), -0.5, -0.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.075);

	glEnd();

	glPopMatrix();

	//Middle Pigu Right Armor 2
	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.075);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02 + 0.2), -0.5, 0.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.02 + 0.15), -0.3, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.1);

	glEnd();

	glPopMatrix();

	//Middle Pigu Right Armor 3
	glColor3f(0.7, 0.95, 0.7);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.1);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02 + 0.15), -0.3, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.02 + 0.15), -0.3, 0.25);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.3);

	glEnd();

	glPopMatrix();

	//Middle Pigu Right Armor 4
	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.3);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02 + 0.15), -0.3, 0.25);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.02 + 0.2), -0.5, 0.4);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.325);

	glEnd();

	glPopMatrix();

	//Middle Pigu Right Armor 5
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslatef(0, 0.42, -0.235);

	glBegin(GL_POLYGON);

	glTexCoord2f(0.0, 1.0);
	glVertex3f((x + 0.02 + 0.1), -0.1, 0.325);

	glTexCoord2f(0.0, 0.0);
	glVertex3f((x + 0.02 + 0.2), -0.5, 0.4);

	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.02 + 0.05), -0.4, 0.475);

	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.02 + 0.075), -0.1, 0.4);

	glEnd();

	glPopMatrix();



}

//Body part
void drawPigu() {
	//Pigu
	glPushMatrix();
	glColor3f(1, 0, 0);
	glRotatef(-90, 1, 0, 0);
	drawCylinder(0.32, 0.235, 0.23);
	glPopMatrix();

}
void drawWaist() {

	//Yao
	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslatef(-0.185, 0.235, -0.185);
	//drawCylinder(0.235, 0.235, 0.25);
	drawRec(0.37, 0.25, 0.27);
	glPopMatrix();
}
void drawChest() {
	//Chest
	glPushMatrix();
	glColor3f(0.2, 0.2, 0.3);
	glTranslatef(-0.235, 0.48, -0.205);
	drawRec(0.47, 0.48, 0.3);
	glPopMatrix();

}
void drawNeck() {

	//Neck
	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslatef(0, 0.96, 0);
	glRotatef(-90, 1, 0, 0);
	drawCylinder(0.06, 0.06, 0.14);
	glPopMatrix();
}

//Head Part

void drawHeadSideUp() {

	//Left side up
	//Front Head 1
	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.175, 0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.2, -0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.075, 0.2, -0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.075, 0.175, 0.025);
	glEnd();


	//Front Head 2
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.075, 0.175, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.15, 0.2, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, 0.175, 0.025);
	glEnd();


	//Front Head 3
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.25, 0.025);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(-0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.15, 0.2, 0.025);
	glEnd();

	//Front Head 4
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.075, 0.2, -0.025);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(-0.1, 0.25, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.05, 0.275, 0.025);

	glEnd();

	//Front Head 5
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 0.275, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0.2, -0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.05, 0.275, 0.025);
	glEnd();



	//Side 1 up
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.15, 0.2, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, 0.175, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1525, 0.175, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1525, 0.2, 0.15);
	glEnd();


	//Side 2 up
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.25, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, 0.2, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1525, 0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1, 0.25, 0.15);
	glEnd();


	//Side 3 up
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.05, 0.275, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1, 0.25, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1, 0.25, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.05, 0.275, 0.15);
	glEnd();

	//Side 4 up
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 0.275, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.05, 0.275, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.05, 0.275, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0, 0.275, 0.15);
	glEnd();

	glPopMatrix();





	//Right side up
	//Front Head 1 Right
	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.175, 0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0.2, -0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.075, 0.175, 0.025);
	glEnd();


	//Front Head 2 Right
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.075, 0.175, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.15, 0.2, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.15, 0.175, 0.025);
	glEnd();


	//Front Head 3 Right
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.25, 0.025);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.15, 0.2, 0.025);
	glEnd();


	//Front Head 4 Right
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.075, 0.2, -0.025);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0.1, 0.25, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.05, 0.275, 0.025);
	glEnd();


	//Front Head 5 Right
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 0.275, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0, 0.2, -0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.075, 0.2, -0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.05, 0.275, 0.025);
	glEnd();


	//Side 1 up Right
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.15, 0.2, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.15, 0.175, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1525, 0.175, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1525, 0.2, 0.15);
	glEnd();


	//Side 2 up Right
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.25, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.15, 0.2, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1525, 0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.25, 0.15);
	glEnd();


	//Side 3 up Right
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.05, 0.275, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1, 0.25, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1, 0.25, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.05, 0.275, 0.15);
	glEnd();


	//Side 4 up Right
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, 0.275, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.05, 0.275, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.05, 0.275, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0, 0.275, 0.15);
	glEnd();


	glPopMatrix();
}
void drawHeadSide() {

	//Side 1
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.15, 0.175, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.15, -0.025, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1525, 0.0, 0.085);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1525, 0.175, 0.085);
	glEnd();


	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.15, 0.175, 0.025);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.15, -0.025, 0.025);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1525, 0.0, 0.085);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1525, 0.175, 0.085);
	glEnd();

	glPopMatrix();

	//Side 2
	glColor3f(0.65, 0.65, 0.65);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1525, 0.175, 0.085);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1525, 0.0, 0.085);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1525, 0.0, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1525, 0.175, 0.15);
	glEnd();


	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1525, 0.175, 0.085);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1525, 0.0, 0.085);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1525, 0.0, 0.15);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1525, 0.175, 0.15);
	glEnd();

	glPopMatrix();


	//Side 3
	glColor3f(0.65, 0.6, 0.65);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1525, 0.175, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1525, 0.0, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1375, 0.0, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1375, 0.175, 0.2);
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1525, 0.175, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1525, 0.0, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1375, 0.0, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1375, 0.175, 0.2);
	glEnd();

	glPopMatrix();

	//Side 4
	glColor3f(0.65, 0.6, 0.65);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1375, 0.175, 0.2);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1375, 0.0, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.11, 0.0, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.11, 0.175, 0.25);
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1375, 0.175, 0.2);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1375, 0.0, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, 0.0, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.11, 0.175, 0.25);
	glEnd();

	glPopMatrix();


	//Side 5 (Back)
	glColor3f(0.65, 0.6, 0.65);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.11, 0.175, 0.25);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.11, 0.0, 0.25);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, 0.0, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.11, 0.175, 0.25);
	glEnd();

	glPopMatrix();

}
void drawHeadArmor() {

	//Eyes
	glColor3f(0.9, 0.9, 0);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(-0.025, 0.1, -0.05);
	glVertex3f(-0.065, 0.12, -0.05);
	glVertex3f(-0.155, 0.15, 0.025);
	glVertex3f(-0.13, 0.1, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(0.025, 0.1, -0.05);
	glVertex3f(0.065, 0.12, -0.05);
	glVertex3f(0.155, 0.15, 0.025);
	glVertex3f(0.13, 0.1, 0);
	glEnd();
	glPopMatrix();

	//Left
	//Head Middle Armor 1
	glColor3f(0.9, 0.85, 0.15);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0.125, -0.075);
	glVertex3f(-0.03, 0.15, -0.075);
	glVertex3f(-0.06, 0.23, -0.075);
	glVertex3f(-0.06, 0.175, -0.075);
	glEnd();
	glPopMatrix();

	//Head Middle Armor 2
	glColor3f(0.9, 0.85, 0.15);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(-0.06, 0.175, -0.075);
	glVertex3f(-0.06, 0.23, -0.075);
	glVertex3f(-0.2, 0.3, -0.025);
	glVertex3f(-0.155, 0.21, -0.025);
	glEnd();
	glPopMatrix();

	//Head Middle Armor 3
	glColor3f(0.8, 0.75, 0.55);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0.125, -0.05);
	glVertex3f(-0.03, 0.23, -0.075);
	glVertex3f(-0.1, 0.27, -0.05);
	glVertex3f(-0.13, 0.33, -0.05);
	glVertex3f(-0.115, 0.32, -0.05);
	glVertex3f(-0.1, 0.2, -0.05);
	glEnd();
	glPopMatrix();







	//Right
	//Head Middle Armor 1
	glColor3f(0.9, 0.85, 0.15);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0.125, -0.075);
	glVertex3f(0.03, 0.15, -0.075);
	glVertex3f(0.06, 0.23, -0.075);
	glVertex3f(0.06, 0.175, -0.075);
	glEnd();
	glPopMatrix();

	//Head Middle Armor 2
	glColor3f(0.9, 0.85, 0.15);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(0.06, 0.175, -0.075);
	glVertex3f(0.06, 0.23, -0.075);
	glVertex3f(0.2, 0.3, -0.025);
	glVertex3f(0.155, 0.21, -0.025);
	glEnd();
	glPopMatrix();

	//Head Middle Armor 3
	glColor3f(0.8, 0.75, 0.55);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0.125, -0.05);
	glVertex3f(0.03, 0.23, -0.075);
	glVertex3f(0.1, 0.27, -0.05);
	glVertex3f(0.13, 0.33, -0.05);
	glVertex3f(0.115, 0.32, -0.05);
	glVertex3f(0.1, 0.2, -0.05);
	glEnd();
	glPopMatrix();


	//Middle Edge
	glColor3f(0.8, 0.75, 0.55);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0.275, 0.025);
	glVertex3f(0.025, 0.2375, -0);
	glVertex3f(0, 0.2, -0.025);
	glVertex3f(-0.025, 0.2375, -0);
	glEnd();
	glPopMatrix();

	//Mouth armor
	glPushMatrix();
	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_QUADS);
	glVertex3f(-0.1, 0.025, -0.025);
	glVertex3f(-0.1, -0.025, -0.025);
	glVertex3f(-0.155, 0.0, 0.025);
	glVertex3f(-0.155, 0.05, 0.025);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-0.155, 0.05, 0.025);
	glVertex3f(-0.155, 0.0, 0.025);
	glVertex3f(-0.155, 0.0, 0.08);
	glVertex3f(-0.155, 0.05, 0.08);
	glEnd();

	glPopMatrix();

}
void drawHead() {

	//Head
	/*glColor3f(1, 0, 0);
	glPushMatrix();
	glTranslatef(-0.1275, 1.1, -0.1275);
	drawRec(0.235, 0.24, 0.235);
	glPopMatrix();*/

	//Face
	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(0, 1.1, -0.128);

	//Down Face
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.1, -0.05);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, -0.1, 0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.15, -0.025, 0.025);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, 0.1, 0.025);
	glEnd();


	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.1, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, -0.1, 0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.15, -0.025, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.15, 0.1, 0.025);
	glEnd();

	//Up Face
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.1, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.175, 0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.15, 0.175, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.15, 0.1, 0.025);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0, 0.1, -0.05);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0, 0.175, 0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.15, 0.175, 0.025);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.15, 0.1, 0.025);
	glEnd();

	glPopMatrix();


	//Left
	//Back Head
	glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();

	glTranslatef(0, 1.1, -0.128);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1525, 0.175, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1525, 0.2, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.1375, 0.2, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.1375, 0.175, 0.2);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1375, 0.175, 0.2);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1375, 0.2, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.11, 0.2, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.11, 0.175, 0.25);
	glEnd();

	//Right
	//Back Head
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1525, 0.175, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1525, 0.2, 0.15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.1375, 0.2, 0.2);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.1375, 0.175, 0.2);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1375, 0.175, 0.2);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1375, 0.2, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, 0.2, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.11, 0.175, 0.25);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.11, 0.175, 0.25);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.11, 0.2, 0.25);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, 0.2, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.11, 0.175, 0.25);
	glEnd();
	glPopMatrix();

	//Top Back Head
	glColor3f(0.65, 0.6, 0.65);
	glPushMatrix();

	glTranslatef(0, 1.1, -0.1285);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1525, 0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1375, 0.2, 0.2);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(-0.1, 0.25, 0.15);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.1, 0.25, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.1375, 0.2, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(-0.11, 0.175, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-0.05, 0.275, 0.15);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1525, 0.2, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1375, 0.2, 0.2);

	glTexCoord2f(0.5, 0.0);
	glVertex3f(0.1, 0.25, 0.15);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.1, 0.25, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.1375, 0.2, 0.2);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, 0.175, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.05, 0.275, 0.15);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-0.05, 0.275, 0.15);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(-0.11, 0.175, 0.25);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.11, 0.175, 0.25);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.05, 0.275, 0.15);
	glEnd();

	glPopMatrix();


	drawHeadSide();
	drawHeadSideUp();
	//drawHeadArmor();
}

//Arm Part
void Thumb()
{
	glPushMatrix();
	glTranslatef(0.428f, -0.3f, -0.08f);
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	//first finger joint
	drawSphere(0.015f);
	glPopMatrix();
	glRotatef(180, 1.0f, 0.0f, 0.0f);
	glRotatef(-90, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.455f, 0.365f, 0.088f); // go origin
	//lies this is finger
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.37f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.1f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.076f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.076f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);

	glEnd();

	glPushMatrix();
	glTranslatef(0.47f, -0.4f, -0.1f);
	glRotatef(-secondFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.4f, 0.1f);
	//2nd finger
	glBegin(GL_QUADS);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);

	glEnd();
	glPopMatrix();
	glPopMatrix();
}
void IndexFinger()
{
	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.088f);
	glRotatef(-firstFingerJoint, 0.0f, 0.0f, 1.0f);

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	//first finger joint
	drawSphere(0.015f);
	glPopMatrix();

	glTranslatef(-0.455f, 0.365f, 0.088f);
	//lies this is finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.37f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.1f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.076f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.076f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);

	glEnd();

	glPushMatrix();
	glTranslatef(0.47f, -0.4f, -0.1f);
	glRotatef(-secondFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.4f, 0.1f);
	//2nd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	//
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);

	glEnd();

	glPushMatrix();			//3rd joint
	glTranslatef(0.47f, -0.43f, -0.1f);
	glRotatef(-thirdFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.43f, 0.1f);
	//3rd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.45f, -0.1f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.45f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.076f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.076f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.076f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.1f);

	glEnd();

	glPopMatrix();		//end of 3rd joint
	glPopMatrix();		//end of 2nd joint
	glPopMatrix();		//end of finger
}
void MiddleFinger()
{
	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.061f);
	glRotatef(-firstFingerJoint, 0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	//first finger joint
	drawSphere(0.015f);
	glPopMatrix();
	glTranslatef(-0.455f, 0.365f, 0.061f);
	//lies this is finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.073f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.073f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.073f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.37f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.073f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.073f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.049f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.049f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.049f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.073f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.049f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.073f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.073f);

	glEnd();

	glPushMatrix();
	glTranslatef(0.47f, -0.4f, -0.1f);
	glRotatef(-secondFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.4f, 0.1f);
	//2nd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.073f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.073f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.073f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.073f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.073f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.049f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.049f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.049f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.073f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.049f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.073f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.073f);

	glEnd();

	glPushMatrix();			//3rd joint
	glTranslatef(0.47f, -0.43f, -0.1f);
	glRotatef(-thirdFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.43f, 0.1f);
	//3rd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.073f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.073f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.45f, -0.073f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.45f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.073f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.073f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.049f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.049f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.049f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.073f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.049f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.073f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.049f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.049f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.073f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.073f);

	glEnd();

	glPopMatrix();		//end of 3rd joint
	glPopMatrix();		//end of 2nd joint
	glPopMatrix();		//end of finger
}
void RingFinger()
{
	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.0365f);
	glRotatef(-firstFingerJoint, 0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	//first finger joint
	drawSphere(0.015f);
	glPopMatrix();
	glTranslatef(-0.455f, 0.365f, 0.0365f);
	//ring finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.047f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.047f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.047f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.37f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.047f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.047f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.024f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.024f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.024f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.047f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.024f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.047f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.047f);

	glEnd();

	glPushMatrix();
	glTranslatef(0.47f, -0.4f, -0.1f);
	glRotatef(-secondFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.4f, 0.1f);
	//2nd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.047f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.047f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.047f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.047f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.047f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.024f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.024f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.024f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.047f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.024f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.047f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.047f);

	glEnd();

	glPushMatrix();			//3rd joint
	glTranslatef(0.47f, -0.43f, -0.1f);
	glRotatef(-thirdFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.43f, 0.1f);
	//3rd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.047f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.047f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.45f, -0.047f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.45f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.047f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.047f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.024f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.024f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.024f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.047f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.45f, -0.024f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.047f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.024f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.024f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.45f, -0.047f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.45f, -0.047f);

	glEnd();

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();			//end of finger
}
void PinkyFinger()
{
	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.012f);
	glRotatef(-firstFingerJoint, 0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	//first finger joint
	drawSphere(0.015f);
	glPopMatrix();
	glTranslatef(-0.455f, 0.365f, 0.012f);
	//ring finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.022f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.022f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.022f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.37f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.022f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.022f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.37f, -0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.37f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.37f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.022f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.022f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.022f);

	glEnd();

	glPushMatrix();
	glTranslatef(0.47f, -0.4f, -0.1f);
	glRotatef(-secondFingerJoint, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.47f, 0.4f, 0.1f);
	//2nd finger
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.022f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.022f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.022f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.43f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.022f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.022f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.44f, -0.4f, -0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.4f, -0.022f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.47f, -0.4f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.47f, -0.43f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.022f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.44f, -0.43f, -0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.47f, -0.43f, -0.0f);

	glEnd();

	glPopMatrix();
	glPopMatrix();			//end of finger
}
void LeftRobotArmMove()
{
	glTranslatef(0.05f, 0.0f, 0.0f);
	glPushMatrix();			//WHOLE HAND
	glTranslatef(0.45f, 0.65f, -0.05f); // moving to top of rect
	if (weaponActivated) {
		if (attackPhase)
		{
			glRotatef(rotateShoulderToSwingXAxis, 1.0f, 0.0f, 0.0f);
		}
		else
		{
			glRotatef(rotateShoulderToGrabZAxis, 0.0f, 0.0f, 1.0f); //125?
			glRotatef(rotateShoulderToGrabYAxis, 0.0f, 1.0f, 0.0f); //90
		}
	}
	else
	{
		glRotatef(leftArmJoint, 1.0f, 0.0f, 0.0f);
	}
	// shoulder joint
	glPushMatrix();

	glColor3f(0.0f, 0.0f, 1.0f);
	drawSphere(0.05);
	glPopMatrix();

	glTranslatef(-0.45f, -0.65f, 0.05f); //whole hand move
	//upper arm
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.4f, 0.6f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.6f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.6f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.6, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.3f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.3f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glEnd();

	glPushMatrix();			// START OF ELBOW JOINT
	//elbow joint
	glTranslatef(0.45f, 0.25f, -0.05f); // moving to top of rect
	if (weaponActivated)
	{
		if (attackPhase == false)
		{
			glRotatef(rotateElbowToGrabXAxis, 1.0f, 0.0f, 0.0f); //125?
			glRotatef(rotateElbowToGrabZAxis, 0.0f, 0.0f, 1.0f);
		}

	}
	else
	{
		glRotatef(subLeftArmJoint, 1.0f, 0.0f, 0.0f);
	}
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	drawSphere(0.05);
	glPopMatrix();

	glTranslatef(-0.45f, -0.25f, 0.05f);
	//forearm
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	//bulk up forearm
	//front left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, 0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	//front right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, 0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	//back left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, -0.15f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, -0.2f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	//back right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, -0.15f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, -0.2f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glEnd();
	//
	glBegin(GL_TRIANGLES);
	//top front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	//top back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, -0.15f);
	//bottom front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, 0.1f);
	//bottom back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, -0.2f);
	glColor3f(0.0f, 0.0f, 0.0f);
	//black shard bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, -0.15f, 0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.1f, 0.1f);
	//black shard top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.45f, 0.1f, 0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.4f, 0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.15f, 0.075f);
	glEnd();

	glPushMatrix();
	glTranslatef(0.45f, -0.22f, -0.05f);
	glRotatef(subLeftArmJoint, 1.0f, 0.0f, 0.0f),
		//wrist joint
		glPushMatrix();
	//glTranslatef(0.45f, -0.22f, -0.05f);
	glColor3f(0.0f, 0.0f, 1.0f);
	drawSphere(0.05);
	glPopMatrix();

	//PALM N ALL
	glTranslatef(-0.45f, 0.22f, 0.05f);
	glBegin(GL_QUADS);
	glColor3f(0.2f, 0.2f, 0.2f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.36f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.43f, -0.36f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.36f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.43f, -0.28f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.48f, -0.28f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.48f, -0.28f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.36f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.48f, -0.36f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.48f, -0.36f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.48f, -0.36f, 0.0f);

	glEnd();

	Thumb();
	IndexFinger();
	MiddleFinger();
	RingFinger();
	PinkyFinger();

	glPopMatrix();		//end of wrist joint
	glPopMatrix();		//end of elbow joint
	glPopMatrix();		//end of shoulder joint

	while (rotateShoulderToGrabYAxis > -90)
	{
		rotateShoulderToGrabYAxis--;
	}
	isShoulderYFinished = true;
	if (isShoulderYFinished)
	{
		while (rotateShoulderToGrabZAxis < 105)
		{
			rotateShoulderToGrabZAxis++;
			while (rotateElbowToGrabZAxis < 15)
			{
				rotateElbowToGrabZAxis++;
			}
			while (rotateElbowToGrabXAxis < 165)
			{
				rotateElbowToGrabXAxis++;
			}

		}
		isShoulderZFinished = true;
	}
}
void RightRobotArm()
{
	glTranslatef(-0.05f, 0.0f, 0.0f);
	glPushMatrix();			//WHOLE HAND
	glTranslatef(0.45f, 0.65f, -0.05f); // moving to top of rect
	glRotatef(rightArmJoint, 1.0f, 0.0f, 0.0f);
	// shoulder joint
	glPushMatrix();

	glColor3f(0.0f, 0.0f, 1.0f);
	drawSphere(0.05);
	glPopMatrix();
	glTranslatef(-0.45f, -0.65f, 0.05f);
	//upper arm
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.4f, 0.6f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.6f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.6f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.6, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.3f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.3f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glEnd();

	glPushMatrix();			// START OF ELBOW JOINT
	//elbow joint
	glTranslatef(0.45f, 0.25f, -0.05f); // moving to top of rect
	glRotatef(subRightArmJoint, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	drawSphere(0.05);
	glPopMatrix();

	glTranslatef(-0.45f, -0.25f, 0.05f);
	//forearm
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	//bulk up forearm
	//front left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, 0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	//front right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, 0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	//back left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, -0.15f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, -0.2f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	//back right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, -0.15f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, -0.2f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glEnd();
	//
	glBegin(GL_TRIANGLES);
	//top front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	//top back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.4f, 0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.5f, 0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.2f, -0.15f);
	//bottom front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, 0.1f);
	//bottom back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.6f, -0.2f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.3f, -0.2f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, -0.2f, -0.2f);
	glColor3f(0.0f, 0.0f, 0.0f);
	//black shard bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.45f, 0.2f, 0.05f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, -0.15f, 0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.1f, 0.1f);
	//black shard top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.45f, 0.1f, 0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.45f, 0.4f, 0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.45f, 0.15f, 0.075f);
	glEnd();

	glPushMatrix();
	glTranslatef(0.45f, -0.22f, -0.05f);
	glRotatef(subRightArmJoint, 1.0f, 0.0f, 0.0f),
		//wrist joint
		glPushMatrix();
	//glTranslatef(0.45f, -0.22f, -0.05f);
	glColor3f(0.0f, 0.0f, 1.0f);
	drawSphere(0.05);
	glPopMatrix();

	//PALM N ALL
	glTranslatef(-0.45f, 0.22f, 0.05f);
	glBegin(GL_QUADS);
	glColor3f(0.2f, 0.2f, 0.2f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.36f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.43f, -0.36f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.36f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.43f, -0.28f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.43f, -0.28f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.48f, -0.28f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.48f, -0.28f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.48f, -0.28f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.48f, -0.36f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.48f, -0.36f, -0.1f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.48f, -0.36f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.43f, -0.36f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.48f, -0.36f, 0.0f);

	glEnd();

	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.08f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	glTranslatef(-0.455f, 0.365f, 0.088f); // go origin
	Thumb();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.088f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	glTranslatef(-0.455f, 0.365f, 0.088f); // go origin
	IndexFinger();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.061f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	glTranslatef(-0.455f, 0.365f, 0.061f);
	MiddleFinger();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.0365f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	glTranslatef(-0.455f, 0.365f, 0.0365f);
	RingFinger();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.455f, -0.365f, -0.012f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	glTranslatef(-0.455f, 0.365f, 0.012f);
	PinkyFinger();
	glPopMatrix();

	glPopMatrix();		//end of wrist joint
	glPopMatrix();		//end of elbow joint
	glPopMatrix();		//end of shoulder joint

}
void drawBothArms()
{
	glTranslatef(0.0f, 0.15f, 0.0f);
	glPushMatrix();
	glTranslatef(-0.2, 0.2, 0.0);
	glTranslatef(0.45, 0.65, 0.05);
	glRotatef(30, 0.0, 0.0, 1.0);
	glTranslatef(-0.45, -0.65, 0.05);
	//drawArm();
	LeftRobotArmMove();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.7, 0.2, 0.0);
	glTranslatef(0.45, 0.65, 0.05);
	glRotatef(-30, 0.0, 0.0, 1.0);
	glTranslatef(-0.45, -0.65, 0.05);
	//drawArm();
	RightRobotArm();
	glPopMatrix();
}

void HalfSword()
{
	glPushMatrix();
	//glScalef(1.2f, 1.2f, 1.2f);
	//glTranslatef(0.0f, 0.4f, 0.4f);

	//THE BASE
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	//
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, -0.1f);
	//
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	//
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.1f);
	glEnd();

	glBegin(GL_TRIANGLES);
	//
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.1f);
	//
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glEnd();


	//BLADEEEE //long to short
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.03f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.8f, -0.03f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.01f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.01f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.01f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, -0.3f, -0.03f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, -0.3f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.8f, -0.03f);



	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.06f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.06f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.06f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.5f, -0.06f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.06f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.04f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.04f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.04f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, 0.0f, -0.06f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.0f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.5f, -0.06f);


	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.09f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.09f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.09f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -0.09f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.09f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.07f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, 0.5f, -0.09f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.5f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -0.09f);
	glEnd();

	//HANDLE
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.02f, 0.8f, -0.05f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.6f, -0.05f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.02f, 0.6f, -0.05f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.02f, 0.6f, -0.05f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.02f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.02f, 0.8f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.02f, 0.8f, -0.05f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.02f, 0.8f, -0.05f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.02f, 0.8f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.02f, 0.8f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.02f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.02f, 0.8f, 0.0f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, 0.8f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.05f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.6f, -0.05f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.02f, 0.6f, 0.0f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.02f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.02f, 0.6f, -0.05f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.02f, 0.6f, -0.05f);

	glEnd();
	glPopMatrix();
}
void BigSword()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.2f, 0.2f);
	HalfSword();
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.05f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	HalfSword();
	glPopMatrix();
	glPopMatrix();
}
void HoldingSword()
{

	glPushMatrix();
	if (attackPhase)
	{
		if (swordOut)
		{
			glTranslatef(0.3f, 0.65f, -0.93f); //sword up position
			glRotatef(60, 1.0f, 0.0f, 0.0f); //sword out up
		}
		else
		{
			glTranslatef(0.55f, 0.15f, -0.5f); //sword down position
			glRotatef(30, 1.0f, 0.0f, 0.0f); //sword out down
		}
		glRotatef(90, 0.0f, 0.0f, 1.0f);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, -0.7f, 0.025f);
		BigSword();
	}
	glPopMatrix();

}

void LeftWing()
{
	glPushMatrix();
	//glScalef(1.2f, 1.2f, 1.2f);
	//glTranslatef(0.0f, 0.4f, 0.4f);

	glPushMatrix();
	//THE BASE
	glTranslatef(0.02f, 0.6f, -0.5f);
	glRotatef(75, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, -0.6f, 0.5f);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, -0.1f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.1f);
	glEnd();

	glBegin(GL_TRIANGLES);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, -0.1f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.1f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.1f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glEnd();

	glPopMatrix();			//end of base

	glPushMatrix();

	glTranslatef(0.075f, 0.6f, -0.5f);

	//BLADEEEE //long to short

	glPushMatrix();
	glRotatef(30, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.075f, -0.6f, 0.5f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.03f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.8f, -0.03f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.01f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.01f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.01f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, -0.3f, -0.03f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, -0.3f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, -0.3f, -0.01f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.8f, -0.01f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.8f, -0.03f);
	glEnd();
	glPopMatrix();			//end of blade (lomg)

	glPushMatrix();
	glRotatef(60, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.075f, -0.6f, 0.5f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.06f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.06f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.06f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.5f, -0.06f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.06f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.04f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.04f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.04f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, 0.0f, -0.06f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.0f, -0.06f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.0f, -0.04f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, -0.5f, -0.04f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, -0.5f, -0.06f);
	glEnd();
	glPopMatrix();			//end of blade (mid)

	glPushMatrix();
	glRotatef(110, 0.0f, 0.0f, 1.0f);
	glTranslatef(-0.075f, -0.6f, 0.5f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.09f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.09f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.09f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -0.09f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.09f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.07f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.55f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.55f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.15f, 0.5f, -0.09f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.15f, 0.5f, -0.09f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.15f, 0.5f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -0.09f);
	glEnd();
	glPopMatrix();			//end of blade(small)

	glPopMatrix();

	glPushMatrix();


	//HANDLE
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	//front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.8f, -0.07f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.6f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	//left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.8f, -0.03f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.8f, -0.07f);
	//back
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.8f, -0.07f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.03f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.0f, 0.8f, -0.03f);
	//top
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.8f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.6f, -0.03f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.02f, 0.8f, -0.03f);
	//right
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, 0.8f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.02f, 0.8f, -0.07f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.02f, 0.6f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.02f, 0.6f, -0.03f);
	//bottom
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.02f, 0.6f, -0.03f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.03f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.0f, 0.6f, -0.07f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.02f, 0.6f, -0.07f);
	glEnd();
	glPopMatrix();			// end of handle
	glPopMatrix();			// end of whoe blade
}
void RightWing()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.05f);
	glRotatef(180, 0.0f, 1.0f, 0.0f);
	LeftWing();
	glPopMatrix();
}
void Wings()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.2f, 0.2f);
	LeftWing();
	RightWing();
	glPopMatrix();
}

void drawBody()
{
	//drawPigu();

	drawWaist();

	drawChest();
	drawChestArmor(0.145);
	drawWaistArmor(0.145);

	drawPiguArmor(0.145);
}

void projection()
{
	glMatrixMode(GL_PROJECTION); // refer to the projection matrix
	glLoadIdentity();

	glTranslatef(ptx, pty, 0.0);
	glRotatef(pRy, 0.0f, 1.0f, 0.0f); // rotate the projection (y-axis only)

	if (isOrtho)
	{
		glOrtho(-4.0, 4.0, -4.0, 4.0, orthoNear, orthoFar); //match the window resolution aspect ratio to prevent distortion
	}
	else {
		//Perspective		
		gluPerspective(10, 1, 1.0, -1.0);
		glFrustum(-10.0, 10.0, -10.0, 10.0, perspecNear, perspecFar);
	}
}

void lighting()
{

	if (isLightOn)
	{
		glEnable(GL_LIGHTING);

		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();

		glTranslatef(posA[0], posA[1], posA[2]);

		drawSphere(0.1);
		glPopMatrix();
	}
	else
	{
		glDisable(GL_LIGHTING); //turning off alll the lightsS
	}

	/*glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();

	drawSphere(0.1);
	glPopMatrix();*/


	//Light 0(zero): red color ambient light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);  //setting ambient
	glLightfv(GL_LIGHT0, GL_POSITION, posA); // setting position
	glEnable(GL_LIGHT0);

	//Light 1(one): yellow diffuse light
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);  //setting ambient
	//glLightfv(GL_LIGHT1, GL_POSITION, posD); // setting position
	//glEnable(GL_LIGHT1);
}

GLuint loadTexture(LPCSTR filename)
{
	//Take from step 1
	GLuint texture = 0;

	//STEP 3 
	//TEXTURE SIZE MUST BE POWER OF 2
	//the3rdsequence.com
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION |
		LR_LOADFROMFILE);

	GetObject(hBMP, sizeof(BMP), &BMP);

	//STEP 4
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST is faster
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	//Take from step 5
	DeleteObject(hBMP);
	return texture;
}

void background()
{
	GLuint texture;

	texture = loadTexture("yellowbanana.bmp");
	drawCube(0.5);
	glDeleteTextures(1, &texture);
}


void display()
{
	glClearColor(0.53, 0.81, 0.92, 0.2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	projection();
	if (!isOrtho)
	{
		glTranslatef(0.0f, 0.0f, 1.5f);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Background
	glPushMatrix();
	if (isOrtho) {
		glScalef(10, 10, 2);
		glTranslatef(0, 0, 2);
	}
	else if(!isOrtho){
		glScalef(80, 80, 2);
		glTranslatef(0, 0, 15);
	}
	//background();
	glPopMatrix();

	glScalef(1.2f, 1.2f, 1.2f);

	glPushMatrix();
	glRotatef(viewAngleX, 0, 1, 0);
	glRotatef(viewAngleY, 1, 0, 0);

	//Add lighting
	lighting();
	glTranslatef(tx, ty, tz);

	//translate for view and adjust position only
	glPushMatrix();
	glTranslatef(0, 0.2, 0);

	//No texture
	drawHeadArmor();

	GLuint textureArr[2];

	if (canloadTexture) {
		textureArr[0] = loadTexture("metalTexture.bmp");
	}

	//Metal texture 1
	//textureArr[0] = loadTexture("metalTexture.bmp");
	drawHead();
	drawNeck();
	drawBody();


	glPopMatrix();

	//draw arm
	if (weaponActivated)
	{
		drawBothArms();
		HoldingSword();
	}
	else {
		drawBothArms();
	}

	if (isWeapon && !attackPhase)
	{
		BigSword();
	}
	else if (!isWeapon && !attackPhase)
	{
		Wings();
	}
	glTranslatef(0, 0.2, -0.05);
	glScalef(1.25, 1.25, 1.25);


	drawBothLegs();
	glDeleteTextures(1, &textureArr[0]);

	//STEP 5: REMOVE TEXTURE INFO
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}
//--------------------------------------------------------------------

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
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
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

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------