#include "icg.h"
#include "constants.h"
#include "ogl.h"
#include "mainProg.h"

//-------------------------------------
//ウィンドウ，カメラ，マウス
ToglWindow		window;
Tmouse			mouse;
MainProg		main_prog;
//-------------------------------------
//フラグ
int imode = -1;
//-------------------------------------

/* local functions */
static void display( void );
static void idle_func ( void );

//----------------------------------------------------------------------
void quit() {
	exit(1);
}
//----------------------------------------------------------------------
static void key_func ( unsigned char key, int x, int y )
{
	if(main_prog.cam.keyFunc(key, x, y)) imode = CAMERA_MODE;
}
//----------------------------------------------------------------------
void mouse_func ( int button, int state, int x, int y )
{
	mouse.updateStatusGL(button, state, x, y);

}
//----------------------------------------------------------------------
void motion_func ( int x, int y )
{
	mouse.setCurrentPosition(x, y);

	if(imode == CAMERA_MODE) 
		main_prog.cam.motionFunc(mouse.getCurPos(), mouse.getMotionVec());

	mouse.setPreviousPosition(x, y);
    glutPostRedisplay();
}

//----------------------------------------------------------------------
static void idle_func ( void )
{

}
//----------------------------------------------------------------------
static void display( void )
{

	glutSetWindow(window.getWindowID());
	window.clear();

	main_prog.draw();

	glutSwapBuffers ();
}
//----------------------------------------------------------------------
void menu_operation(int val) {

	if(val == QUIT) quit();

}
//----------------------------------------------------------------------
void cam_menu_operation(int val)
{
	imode = CAMERA_MODE;
	main_prog.cam.menuOperation(val);
}
//----------------------------------------------------------------------
void setGlutMenu() {
	
	int cam_menu = main_prog.cam.createGlutMenu(cam_menu_operation);


    int main_menu = glutCreateMenu(menu_operation);

	glutAddSubMenu("camera", cam_menu);
    glutAddMenuEntry("quit", QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------
void initWindow(int w, int h) {
	//---------------------------------------------
	//ウィンドウの初期化
	window.setSize(w, h);
	window.setTitle(WINDOW_TITLE);
	window.setClearColor(CLEAR_COLOR);
	window.create();
}
//----------------------------------------------------------------------
void initGlut()
{

	//---------------------------------------------
	//メニューの初期化
	setGlutMenu();
    
    glutKeyboardFunc ( key_func );
    glutMouseFunc ( mouse_func );
    glutMotionFunc ( motion_func );
    glutDisplayFunc ( display );
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

}
//----------------------------------------------------------------------

int main ( int argc, char ** argv )
{
    glutInit ( &argc, argv );

	initWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	initGlut();
	main_prog.init(window.getWindowSize());

	glutMainLoop ();

    exit ( 0 );
}