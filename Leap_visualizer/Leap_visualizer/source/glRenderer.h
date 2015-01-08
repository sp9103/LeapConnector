#include "Thread.h"
#include "define.h"

#include <mutex>
#include <Windows.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>

static bool streamOpen = true;

class glRenderer
{
public:
	glRenderer(void);
	~glRenderer(void);

	//Initialize.
	//Create Thread, Initialize variable
	//set number of kinect to rendering.
	void InitializeRenderer(char *WindowName);
	void DeInitializeRenderer();

	//Check Window close in thread.
	int CheckWindowClose();

	/*Copy & send to thread*/
	//���� �ȿ��� �� �����Ӹ��� ȣ���������.
	void SetHandInfo(HandStruct HandInfo);

	//Thread�� ���� ����Ǳ� ��ٸ�
	void WaitUntilThreadDead();
	void WaitUntilThreadInit();

private:
	static void key( GLFWwindow* window, int k, int s, int action, int mods );
	static void scroll_callback(GLFWwindow* window, double x, double y);

	void draw(HandsStruct HandData);
	void drawFrame(float len);
	void drawFloor();
	void drawSphere(double r, int lats, int longs, GLfloat *pColor);
	void drawhand(HandStruct hand);
	void DrawSkelBone(Joint* pJoints, cv::Point3f* pJointPoints, JointType joint0, JointType joint1, GLfloat *t_Color);

	static void reshape( GLFWwindow* window, int width, int height );

	void init();

	GLFWwindow* m_window;
	GLfloat angle;

	int m_width, m_height;
	int m_numKinect;
	unsigned m_floorTexID;

	void Display(HandsStruct JointData);

	//For Multi Threading
	CRITICAL_SECTION m_cs;
	Thread m_glThread;

	bool m_EndThread;
	static UINT WINAPI renderThread(LPVOID param); // ������ �Լ�.

	//Shared Variable. ������ ������ �� ���� ��������. - ����ü ť�� �����ұ�...
	HandStruct mHandInfo;					//�����忡 �Ѱ��� ����.

	GLfloat view_rotx, view_roty, view_rotz;
	GLfloat view_tranx, view_trany, view_tranz;
	bool m_EnableThread;
	bool m_InitCheck;

	char WindowName[256];
};

