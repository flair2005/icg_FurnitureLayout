#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

#include "doba-cg.h"
#include "objectManager.h"
//============================================

#define TIMER_INTERVAL 1

#define KEY_ESC 27

#define CAMERA_OPERATION 10

#define MODE_SELECTION_RADIO_GROUP	100

#define QUIT             9999
//============================================

class TmainWindow : public GlutWindow{
public:

	int          height, width;
	int          initPositionX, initPositionY;

	GlutMaster *master;

	TglutCamera	*cam;
	int			imode;
	Tmouse		*mouse;
	Tcolor3<float> bg_col;

	TobjectManager	*objectManager;


	static TmainWindow *that;


	///GLUIópÇÃïœêî
	static int live_int;
	static float live_float;
	static char  live_text[1024];


	TmainWindow(GlutMaster * glutMaster,
			  Tvector2<int>		size,
			  Tvector2<int>	   init_pos,
			  char * title,
			  char * obj_file);
	~TmainWindow();

	void CallBackDisplayFunc(void);
	void CallBackIdleFunc();
	void CallBackKeyboardFunc(unsigned char key, int x, int y);
	void CallBackMotionFunc(int x, int y);
	void CallBackMouseFunc(int button, int state, int x, int y);


	static void menu_operation(int val);
	static void menu_operation_glui(int val);
	void setGlutMenu();
	void setGluiMenu();

};

#endif
