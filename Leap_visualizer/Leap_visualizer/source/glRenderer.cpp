//#include "stdafx.h"
#include "glRenderer.h"


glRenderer::glRenderer(void)
{
	view_rotx = 10.f, view_roty = 180.f, view_rotz = 0.f;
	view_tranx = 0.0f, view_trany = -5.0f, view_tranz = -40.f;

	//Thread Run!
	m_EndThread = false;
	m_EnableThread = false;
	InitializeCriticalSection(&m_cs);

	m_InitCheck = false;

}


glRenderer::~glRenderer(void)
{
	m_EnableThread = false;
}

void glRenderer::InitializeRenderer(char *name){

	strcpy(WindowName, name);

	printf("Start rendering thread..\n");
	m_EnableThread = true;
	m_glThread.StartThread(renderThread, this);
}

int glRenderer::CheckWindowClose(){
	return /*glfwWindowShouldClose(m_window)*/m_EndThread;
}

void glRenderer::DeInitializeRenderer(){
	// Terminate GLFW
	glfwTerminate();

	//Thread Kill
	m_EnableThread = false;

	// Exit program
	exit( EXIT_SUCCESS );
}

/* change view angle, exit upon ESC */
void glRenderer::key( GLFWwindow* window, int k, int s, int action, int mods )
{
	glRenderer* tClass = reinterpret_cast<glRenderer *>(glfwGetWindowUserPointer(window));
	switch (k) {
	case GLFW_KEY_Z:
		if( mods & GLFW_MOD_SHIFT )
			tClass->view_rotz -= 5.0;
		else
			tClass->view_rotz += 5.0;
		break;
	case GLFW_KEY_ESCAPE:
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			tClass->m_EnableThread = false;
			break;
		}
	case GLFW_KEY_UP:
		tClass->view_rotx += 5.0;
		break;
	case GLFW_KEY_DOWN:
		tClass->view_rotx -= 5.0;
		break;
	case GLFW_KEY_LEFT:
		tClass->view_roty += 5.0;
		break;
	case GLFW_KEY_RIGHT:
		tClass->view_roty -= 5.0;
		break;
	case GLFW_KEY_SPACE:
		tClass->view_rotx = 20.f, tClass->view_roty = 180.f, tClass->view_rotz = 0.f;
		break;
	default:
		return;
	}
}

void glRenderer::scroll_callback(GLFWwindow* window, double x, double y){
	static const float tran_acc = 2.0f;
	glRenderer* tClass = reinterpret_cast<glRenderer *>(glfwGetWindowUserPointer(window));
	//view_tranx += y;
	tClass->view_trany += tran_acc/8.f*y;
	tClass->view_tranz += tran_acc*y;
}

/* new window size */
void glRenderer::reshape( GLFWwindow* window, int width, int height )
{
	GLfloat h = (GLfloat) height / (GLfloat) width;
	GLfloat xmax, znear, zfar;

	znear = 5.0f;
	zfar  = 200.0f;
	xmax  = znear * 0.5f;

	glViewport( 0, 0, (GLint) width, (GLint) height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar );
}

/*********************************************************************************
* Draw x, y, z axis of current frame on screen.
* x, y, and z are corresponded Red, Green, and Blue, resp.
**********************************************************************************/
void glRenderer::drawFrame(float len)
{
	glDisable(GL_LIGHTING);		// Lighting is not needed for drawing axis.
	glBegin(GL_LINES);			// Start drawing lines.
	glColor3d(1,0,0);			// color of x-axis is red.
	glVertex3d(0,0,0);			
	glVertex3d(len,0,0);		// Draw line(x-axis) from (0,0,0) to (len, 0, 0). 
	glColor3d(0,1,0);			// color of y-axis is green.
	glVertex3d(0,0,0);			
	glVertex3d(0,len,0);		// Draw line(y-axis) from (0,0,0) to (0, len, 0).
	glColor3d(0,0,1);			// color of z-axis is  blue.
	glVertex3d(0,0,0);
	glVertex3d(0,0,len);		// Draw line(z-axis) from (0,0,0) - (0, 0, len).
	glEnd();					// End drawing lines.
}

/*********************************************************************************
* Draw floor on 3D plane.
**********************************************************************************/
void glRenderer::drawFloor()
{  
	glDisable(GL_LIGHTING);
	// Set color of the floor.
	// Assign checker-patterned texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_floorTexID );

	// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
	glBegin(GL_POLYGON);
	glTexCoord2d(0,0);
	glVertex3d(-12,0,-12);		// Texture's (0,0) is bound to (-12,-0.1,-12).
	glTexCoord2d(1,0);
	glVertex3d( 12,0,-12);		// Texture's (1,0) is bound to (12,-0.1,-12).
	glTexCoord2d(1,1);
	glVertex3d( 12,0, 12);		// Texture's (1,1) is bound to (12,-0.1,12).
	glTexCoord2d(0,1);
	glVertex3d(-12,0, 12);		// Texture's (0,1) is bound to (-12,-0.1,12).
	glEnd();

	glDisable(GL_TEXTURE_2D);	
	drawFrame(5);				// Draw x, y, and z axis.
}

void glRenderer::drawSphere(double r, int lats, int longs, GLfloat *pColor) {
	int i, j;

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pColor);
	for(i = 0; i <= lats; i++) {
		double lat0 = PI * (-0.5 + (double) (i - 1) / lats);
		double z0  = r*sin(lat0);
		double zr0 =  r*cos(lat0);

		double lat1 = PI * (-0.5 + (double) i / lats);
		double z1 = r*sin(lat1);
		double zr1 = r*cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			double lng = 2 * PI * (double) (j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}
}

/* program & OpenGL initialization */
void glRenderer::init()
{

	static GLfloat pos[4] = {5.f, 5.f, 10.f, 0.f};
	static GLfloat red[4] = {0.8f, 0.1f, 0.f, 1.f};
	static GLfloat green[4] = {0.f, 0.8f, 0.2f, 1.f};
	static GLfloat blue[4] = {0.2f, 0.2f, 1.f, 1.f};

	// Set up OpenGL state
	glShadeModel(GL_SMOOTH);         // Set Smooth Shading
	glEnable(GL_DEPTH_TEST);         // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);          // The Type Of Depth Test To Do
	// Use perspective correct interpolation if available
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// Define lighting for the scene
	float lightDirection[]   = {1.0, 1.0, 1.0, 0};
	float ambientIntensity[] = {0.1, 0.1, 0.1, 1.0};
	float lightIntensity[]   = {0.9, 0.9, 0.9, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
	glEnable(GL_LIGHT0);

	// initialize floor
	{
		// After making checker-patterned texture, use this repetitively.

		// Insert color into checker[] according to checker pattern.
		const int size = 8;
		unsigned char checker[size*size*3];
		for( int i=0; i < size*size; i++ )
		{
			if (((i/size) ^ i) & 1)
			{
				checker[3*i+0] = 200;
				checker[3*i+1] = 32;
				checker[3*i+2] = 32;
			}
			else
			{
				checker[3*i+0] = 200;
				checker[3*i+1] = 200;
				checker[3*i+2] = 32;
			}
		}

		// Make texture which is accessible through floorTexID. 
		glGenTextures( 1, &m_floorTexID );				
		glBindTexture(GL_TEXTURE_2D, m_floorTexID);		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);
	}

	glEnable(GL_NORMALIZE);
}

void glRenderer::Display(HandsStruct HandData)
{
	// Draw
	draw(HandData);

	// Swap buffers
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

/* OpenGL draw function & timing */
//거꾸로 생각해야되네
void glRenderer::draw(HandsStruct HandData)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0.0f, -9.5f, 27.5f);
	//drawFloor();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	if(HandData.bleft)
		drawhand(HandData.LeftHand);
	if(HandData.bright)
		drawhand(HandData.RightHand);

	drawFrame(5);
	glMatrixMode( GL_MODELVIEW );				//ModelView가 카메라
	glLoadIdentity();
	glTranslatef( view_tranx, view_trany, view_tranz );			// 카메라 관련?
	glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glRotatef(view_rotz, 0.0, 0.0, 1.0);
	glTranslatef(0.0f, 10.0f, -30.0f);
}

void glRenderer::DrawSkelBone(Joint* pJoints, cv::Point3f* pJointPoints, JointType joint0, JointType joint1, GLfloat *t_Color){
	static const float sphereRad = .2;
	static const float cylinderRad = .3;
	static const int tSlice = 10;

	cv::Point3f temp = pJointPoints[joint1] - pJointPoints[joint0];

	//handle the degenerate case of z1 == z2 with an approximation
	if(temp.z == 0)
		temp.z = .0001;

	float v = sqrt( temp.x*temp.x + temp.y*temp.y + temp.z*temp.z );
	float ax = 57.2957795*acos( temp.z/v );
	if ( temp.z < 0.0 )
		ax = -ax;
	float rx = -temp.y*temp.z;
	float ry = temp.x*temp.z;

	GLUquadric *quadric;
	quadric = gluNewQuadric();

	glPushMatrix();
	glTranslatef(pJointPoints[joint0].x, pJointPoints[joint0].y, pJointPoints[joint0].z);
	gluSphere(quadric, sphereRad, tSlice, tSlice);
	glRotatef(ax, rx, ry, 0.0);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluCylinder(quadric, cylinderRad, cylinderRad, v, tSlice, tSlice);
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

void glRenderer::drawhand(HandStruct hand){
	int32_t hid = hand.hid;
	GLfloat tColor[4] = {(hid*37)%256/255.f, (hid*113)%256/255.f, (hid*71)%256/255.f};

	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tColor);

	/////////////////////Draw Joint////////////////////////////////////
	cv::Point3f gljointpoints[JointType_Count];
	for(int i = 0; i < JointType_Count; i++)
		glTransformCoordinate(tBody.JointPos[i], &gljointpoints[i]);			//Transformation -> Image coordinate
	
	// Torso
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_Head, JointType_Neck, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_Neck, JointType_SpineShoulder, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_SpineShoulder, JointType_SpineMid, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_SpineMid, JointType_SpineBase, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_SpineShoulder, JointType_ShoulderRight, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_SpineShoulder, JointType_ShoulderLeft, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_SpineBase, JointType_HipRight, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_SpineBase, JointType_HipLeft, tColor);

	// Right Arm
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_ShoulderRight, JointType_ElbowRight, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_ElbowRight, JointType_WristRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristRight, JointType_HandRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_HandRight, JointType_HandTipRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristRight, JointType_ThumbRight, tColor);

	// Left Arm
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_ShoulderLeft, JointType_ElbowLeft, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_ElbowLeft, JointType_WristLeft, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristLeft, JointType_HandLeft, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_HandLeft, JointType_HandTipLeft, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_WristLeft, JointType_ThumbLeft, tColor);

	// Right Leg
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_HipRight, JointType_KneeRight, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_KneeRight, JointType_AnkleRight, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_AnkleRight, JointType_FootRight, tColor);

	// Left Leg
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_HipLeft, JointType_KneeLeft, tColor);
	DrawSkelBone(tBody.JointPos, gljointpoints, JointType_KneeLeft, JointType_AnkleLeft, tColor);
	//DrawSkelBone(tBody.JointPos, gljointpoints, JointType_AnkleLeft, JointType_FootLeft, tColor);
	///////////////////////////////////////////////////////////////////

	glPopMatrix();


}

void glRenderer::SetHandInfo(HandStruct HandInfo){
	EnterCriticalSection(&m_cs);
	memcpy(&mHandInfo, &HandInfo, sizeof(HandInfo));
	LeaveCriticalSection(&m_cs);
}

UINT WINAPI glRenderer::renderThread(LPVOID param){
	glRenderer *t_glRenderer = (glRenderer *)param;
	//SkeletonInfo *threadBodyInfo;
	HandsStruct threadHandInfo;

	//Thread Initialize..
	printf("Thread Initialize start...\n");
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit( EXIT_FAILURE );
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 160);

	t_glRenderer->m_window = glfwCreateWindow( 640, 480, t_glRenderer->WindowName, NULL, NULL );
	glfwSetWindowUserPointer(t_glRenderer->m_window, t_glRenderer);
	if (!t_glRenderer->m_window)
	{
		fprintf( stderr, "Failed to open GLFW window\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	// Set callback functions

	glfwSetFramebufferSizeCallback(t_glRenderer->m_window, t_glRenderer->reshape);
	glfwSetKeyCallback(t_glRenderer->m_window, t_glRenderer->key);
	glfwSetScrollCallback(t_glRenderer->m_window, scroll_callback);

	glfwMakeContextCurrent(t_glRenderer->m_window);
	glfwSwapInterval( 1 );

	glfwGetFramebufferSize(t_glRenderer->m_window, &t_glRenderer->m_width, &t_glRenderer->m_height);
	reshape(t_glRenderer->m_window, t_glRenderer->m_width, t_glRenderer->m_height);

	// Parse command-line options
	t_glRenderer->init();
	printf("Thread Initialize complete!\n");
	t_glRenderer->m_InitCheck = true;

	//Thread run...
	while(!glfwWindowShouldClose(t_glRenderer->m_window) || t_glRenderer->m_EnableThread){ 

		EnterCriticalSection(&t_glRenderer->m_cs);
		memcpy(&threadHandInfo, &t_glRenderer->mHandInfo, sizeof(HandsStruct)); 
		LeaveCriticalSection(&t_glRenderer->m_cs);

		t_glRenderer->Display(threadHandInfo);
	}

	//Thread exit...
	t_glRenderer->m_EndThread = true;

	return 0;
}

void glRenderer::WaitUntilThreadDead(){
	m_EnableThread = false;

	while(m_EndThread){
		Sleep(100);
	}
}

void glRenderer::WaitUntilThreadInit(){
	while(!m_InitCheck){
		Sleep(10);
	}
}