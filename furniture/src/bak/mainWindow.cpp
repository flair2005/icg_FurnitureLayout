
#include "mainWindow.h"
#include <GL/glui.h>

TmainWindow *TmainWindow::that = NULL;
int		TmainWindow::live_int = 0;
float	TmainWindow::live_float = 0.0;
char	TmainWindow::live_text[1024];
//----------------------------------------------------------------------
TmainWindow::TmainWindow(GlutMaster * glutMaster,
			  Tvector2<int>		size,
			  Tvector2<int>	   init_pos,
              char * title,
			  char * obj_file) {

	TmainWindow::that = this;

	this->master = glutMaster;
	initPositionX = init_pos.x;
	initPositionY = init_pos.y;
	width = size.x;
	height = size.y;

	//------------------------------------------------------------
	//ウィンドウの初期化
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA);

	glutInitWindowPosition ( initPositionX, initPositionY );
	glutInitWindowSize ( width, height);
	glutMaster->CallGlutCreateWindow(title, this);
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//------------------------------------------------------------
	//背景色の設定
	bg_col.assign(0.2, 0.4, 0.5);

	//------------------------------------------------------------
	//カメラの生成
	cam = new TglutCamera(Tvector3<float>(10.0, 0.0, 0.0) //視点
						 ,Tvector3<float>(0.0, 0.0, 0.0) //注視点
						 ,0.02f //Near平面までの距離
						 ,10000.0f //Far平面までの距離
						 ,TO_RAD(60.0f) //視野角[rad]
						 ,Tvector2<int>(width, height)); //ウィンドウサイズ


	//------------------------------------------------------------
	//マウスの生成
	mouse = new Tmouse();

	//------------------------------------------------------------
	//objファイルの読み込み
	objectManager = new TobjectManager();
	if(::isFileExist(obj_file, "r")) {
		printf("reading obj file: %s\n", obj_file);
		objectManager->readObjectFromObjFile(obj_file);
		TlightParam light(Tvector3<float>(1., 1., 1.), Tcolor3<float>(1., 1., 1.), 1.0, 0.0);
		for(int i=0; i<objectManager->getNumObject(); i++) {
			Tobject *obj = objectManager->getObjectAddress(i);
			obj->scale(Tvector3<float>(0.1, 0.1, 0.1));
			printf("%f, %f, %f\n", obj->getReflectance().r, obj->getReflectance().g, obj->getReflectance().b);
		}
		objectManager->shadeObjects(light);
	} else {
		fprintf(stderr, "no such obj file: %s\n", obj_file);
	}

	setGlutMenu();
	setGluiMenu();

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}
//----------------------------------------------------------------------
TmainWindow::~TmainWindow(){

   glutDestroyWindow(GetWindowID());
   delete cam;
   delete mouse;

   delete objectManager;
}

//----------------------------------------------------------------------

void TmainWindow::menu_operation(int val) {
//    time_t ttt;

	if(that->cam->setMode(val) == true) {
		//カメラ用メニューコマンドの場合
		//カメラ操作モードに設定
        that->imode = CAMERA_OPERATION;
        that->CallBackDisplayFunc();
	} else if(val == QUIT) {
        exit(1);
    }
}
//----------------------------------------------------------------------
void TmainWindow::menu_operation_glui(int val) {

	switch(val) {
		//モード選択
		case MODE_SELECTION_RADIO_GROUP:
			switch(live_int) {
				case 0:
					that->imode = CAMERA_OPERATION;
					break;
				case 1:
					that->imode = -1;
					break;
			}
			break;

		//終了
		case QUIT:
			exit(1);
	}

}
//----------------------------------------------------------------------
void TmainWindow::CallBackKeyboardFunc( unsigned char key, int x, int y )
{
	if(cam->keyFunc(key, x, y) == true) {
		//カメラ用キーコマンドの場合
		//カメラ操作モードに設定
		imode = CAMERA_OPERATION;
		CallBackDisplayFunc();
	} else if(key == KEY_ESC) {
		menu_operation(QUIT);
	}
}
//----------------------------------------------------------------------
void TmainWindow::CallBackMouseFunc ( int button, int state, int x, int y )
{
	mouse->updateStatusGL(button, state, x, y);
}
//----------------------------------------------------------------------
void TmainWindow::CallBackMotionFunc ( int x, int y )
{
	mouse->setCurrentPosition(x, y);
	if(mouse->isDragging() && imode == CAMERA_OPERATION) {
		//カメラ操作モードの場合，マウスの動きに応じてカメラを移動
		cam->motionFunc(mouse->getCurPos(), mouse->getMotionVec());
    }
	mouse->setPreviousPosition(x, y);
    CallBackDisplayFunc();
}
//----------------------------------------------------------------------
void TmainWindow::CallBackIdleFunc() {
	CallBackDisplayFunc();
}

//----------------------------------------------------------------------
void TmainWindow::CallBackDisplayFunc( void )
{
	//------------------------------------------------------
	//ウィンドウのアクティブに
	glutSetWindow ( GetWindowID() );

	//------------------------------------------------------
	//カメラの設定と画面のクリア
	cam->glCamera();

    glClearColor ( bg_col.r,bg_col.g, bg_col.b, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//------------------------------------------------------
	//物体の描画
	Tobject *obj;
	for(int i=0; i<objectManager->getNumObject(); i++) {
		obj = objectManager->getObjectAddress(i);
		obj->draw();
	}

    glutSwapBuffers ();
}
//----------------------------------------------------------------------
void TmainWindow::setGlutMenu() {
	//サブメニューの登録とメインメニューの設定
	//カメラ用のサブメニューの生成
    int move_sub_menu = glutCreateMenu(this->menu_operation);
	cam->setMenuEntry();

	int main_menu = glutCreateMenu(menu_operation);
    glutAddSubMenu("view", move_sub_menu);
    glutAddMenuEntry("quit", QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------
void TmainWindow::setGluiMenu() {
	GLUI *glui = GLUI_Master.create_glui("glui", 0, this->width+20, 0);
	GLUI_Master.set_glutIdleFunc(master->CallBackIdleFunc);

	GLUI_RadioGroup *rg;
	GLUI_Checkbox *chk_box;
	GLUI_Button *button;
	GLUI_Rollout *rollout;
	GLUI_Panel		*sub_panel;
	GLUI_Spinner  *spinner;
	GLUI_Panel		*top_panel;
	GLUI_EditText	*edit_text;

	top_panel = glui->add_panel("");

	//モード選択メニュー
	sub_panel = glui->add_panel_to_panel(top_panel, "Mode", true);
	rg = glui->add_radiogroup_to_panel(sub_panel, &live_int, MODE_SELECTION_RADIO_GROUP, menu_operation_glui);
	glui->add_radiobutton_to_group(rg, "move camera");
	glui->add_radiobutton_to_group(rg, "n/a");
	rg->set_int_val(1);


	//終了ボタン
	glui->add_button("quit", QUIT, menu_operation_glui);

	glui->set_main_gfx_window(GetWindowID());
}
//----------------------------------------------------------------------

