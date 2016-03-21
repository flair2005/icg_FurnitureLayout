
#include "objViewer.h"
#include <GL/glui.h>

TobjViewer *TobjViewer::that = NULL;
int		TobjViewer::live_int = 0;
float	TobjViewer::live_float = 0.0;
char	TobjViewer::live_text[1024];
//----------------------------------------------------------------------
TobjViewer::TobjViewer(GlutMaster * glutMaster,
			  Tvector2<int>		size,
			  Tvector2<int>	   init_pos,
              char * title,
			  char * obj_file) {

	TobjViewer::that = this;

	this->master = glutMaster;
	initPositionX = init_pos.x;
	initPositionY = init_pos.y;
	width = size.x;
	height = size.y;

	//------------------------------------------------------------
	//�E�B���h�E�̏�����
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA);

	glutInitWindowPosition ( initPositionX, initPositionY );
	glutInitWindowSize ( width, height);
	glutMaster->CallGlutCreateWindow(title, this);
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//------------------------------------------------------------
	//�w�i�F�̐ݒ�
	bg_col.assign(0.2, 0.4, 0.5);

	//------------------------------------------------------------
	//�J�����̐���
	//cam = new TglutCamera(Tvector3<float>(10.0, 0.0, 0.0) //���_
	//					 ,Tvector3<float>(0.0, 0.0, 0.0) //�����_
	//					 ,0.02f //Near���ʂ܂ł̋���
	//					 ,10000.0f //Far���ʂ܂ł̋���
	//					 ,TO_RAD(60.0f) //����p[rad]
	//					 ,Tvector2<int>(width, height)); //�E�B���h�E�T�C�Y
	cam = new Tcamera(Tvector3<float>(10.0, 0.0, 0.0) //���_
						 ,Tvector3<float>(0.0, 0.0, 0.0) //�����_
						 ,0.02f //Near���ʂ܂ł̋���
						 ,10000.0f //Far���ʂ܂ł̋���
						 ,TO_RAD(60.0f) //����p[rad]
						 ,Tvector2<int>(width, height)); //�E�B���h�E�T�C�Y


	//------------------------------------------------------------
	//TlightParam�̐���
	sun = new TlightParam(Tvector3<float>(0.0, 0.0, 1.0), Tcolor3<float>(1.0, 1.0, 1.0), 1.0, 0.1);

	//------------------------------------------------------------
	//��
	sky = new TrealSky();
	sky->setIntensityScale(0.001);
	skyDome = new TskyDome(500.0, 18, 36, sun->getDirection(), *sky);

	//------------------------------------------------------------
	//�_�����̐���
	pointLight = new TpointLight();
	//------------------------------------------------------------
	//���s�����̐���
	parallelLight = new TparallelLight(Tvector3<float>(0.0, 0.0, 1.0));
	glui_menu.push_back(new TgluiCameraMenu(cam));


	//------------------------------------------------------------
	//�}�E�X�̐���
	mouse = new Tmouse();

	//------------------------------------------------------------
	//obj�t�@�C���̓ǂݍ���
	objectManager = new TobjectManager();
	if(::isFileExist(obj_file, "r")) {
		printf("reading obj file: %s\n", obj_file);
		objectManager->readObjectFromObjFile(obj_file);
		for(int i=0; i<objectManager->getNumObject(); i++) {
			Tobject *obj = objectManager->getObjectAddress(i);
			obj->scale(Tvector3<float>(0.1, 0.1, 0.1));
			printf("n vtx: %d\n", obj->getNumVertex());
			printf("n tri: %d\n", obj->getNumTriangle());
		}
		shadeObjects();
	} else {
		fprintf(stderr, "no such obj file: %s\n", obj_file);
	}
	//------------------------------------------------------------
	//�o�͉摜
	outputImage = new Timage(this->width, this->height, 3);
	outputImage->setFileName("test.bmp");

	//------------------------------------------------------------
	//���j���[
	menu_name.push_back("camera");
	glui_menu.push_back(new TgluiLightParamMenu(sun));
	menu_name.push_back("sun");
	glui_menu.push_back(new TgluiRealSkyMenu((TrealSky*)sky));
	menu_name.push_back("sky");
	glui_menu.push_back(new TgluiPointLightMenu(pointLight));
	menu_name.push_back("point light");
	glui_menu.push_back(new TgluiParallelLightMenu(parallelLight));
	menu_name.push_back("parallel light");
	glui_menu.push_back(new TgluiSaveImageMenu(outputImage));
	menu_name.push_back("output image");
	sun_menu_id = 1;
	sky_menu_id = 2;

	setGlutMenu();
	setGluiMenu();

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}
//----------------------------------------------------------------------
TobjViewer::~TobjViewer(){

   glutDestroyWindow(GetWindowID());
   delete cam;
   delete sun;
   delete sky;
   delete skyDome;
   delete pointLight;
   delete parallelLight;
   delete mouse;

   delete outputImage;

   delete objectManager;

   for(unsigned int i=0; i<glui_menu.size(); i++)
	   delete glui_menu.at(i);

}

//----------------------------------------------------------------------

void TobjViewer::menu_operation(int val) {

	if(val == QUIT) {
        exit(1);
    }
}
//----------------------------------------------------------------------
void TobjViewer::menu_operation_glui(int val) {

	for(unsigned int i=0; i<that->glui_menu.size(); i++)
		that->glui_menu.at(i)->gluiMenuOperation(val);

	if(that->glui_menu.at(that->sun_menu_id)->isOperated() ||
		that->glui_menu.at(that->sky_menu_id)->isOperated() ) {
		that->skyDome->updateColor(that->sun->getDirection(), *(that->sky));
		that->sun->setColor(that->sky->getDirectSunColor(1.0, that->sun->getColor()));
	}

	that->gluiPointLight.menuOperation(val);

	switch(val) {
		//�I��
		case QUIT:
			exit(1);
	}

	glutPostRedisplay();

}
//----------------------------------------------------------------------
void TobjViewer::CallBackKeyboardFunc( unsigned char key, int x, int y )
{
	for(unsigned int i=0; i<glui_menu.size(); i++)
		glui_menu.at(i)->keyFunc(key, x, y);
	if(key == KEY_ESC) {
		menu_operation(QUIT);
	}
}
//----------------------------------------------------------------------
void TobjViewer::CallBackMouseFunc ( int button, int state, int x, int y )
{
	for(unsigned int i=0; i<glui_menu.size(); i++)
		glui_menu.at(i)->mouseFunc(button, state, x, y);

	if(glui_menu.at(sun_menu_id)->isOperated() ||
		glui_menu.at(sky_menu_id)->isOperated() ) {
		skyDome->updateColor(sun->getDirection(), *(sky));
		sun->setColor(sky->getDirectSunColor(1.0, sun->getColor()));
	}

	mouse->updateStatusGL(button, state, x, y);

	glutPostRedisplay();
}
//----------------------------------------------------------------------
void TobjViewer::CallBackMotionFunc ( int x, int y )
{
	mouse->setCurrentPosition(x, y);
//	cam->motionFunc(mouse->getCurPos(), mouse->getMotionVec());
	for(unsigned int i=0; i<glui_menu.size(); i++)
		glui_menu.at(i)->motionFunc(x, y, mouse->getPrevPos().x, mouse->getPrevPos().y);

	if(glui_menu.at(sun_menu_id)->isOperated() ||
		glui_menu.at(sky_menu_id)->isOperated() ) {
		skyDome->updateColor(sun->getDirection(), *(sky));
		sun->setColor(sky->getDirectSunColor(1.0, sun->getColor()));
	}

	mouse->setPreviousPosition(x, y);
    CallBackDisplayFunc();
}
//----------------------------------------------------------------------
void TobjViewer::CallBackIdleFunc() {
	for(unsigned int i=0; i<glui_menu.size(); i++)
		glui_menu.at(i)->idleFunc();
	CallBackDisplayFunc();
}

//----------------------------------------------------------------------
void TobjViewer::CallBackDisplayFunc( void )
{
	//------------------------------------------------------
	//�E�B���h�E�̃A�N�e�B�u��
	glutSetWindow ( GetWindowID() );

	//------------------------------------------------------
	//�J�����̐ݒ�Ɖ�ʂ̃N���A
	cam->glCamera();

    glClearColor ( bg_col.r,bg_col.g, bg_col.b, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//------------------------------------------------------
	//���̂̕`��
	shadeObjects();
	Tobject *obj;
	for(int i=0; i<objectManager->getNumObject(); i++) {
		obj = objectManager->getObjectAddress(i);
		obj->draw();
		glColor3f(1., 0., 0.);
	}
	//-----------------------------------------------------
	Tvector3<float> p = gluiPointLight.getPosition();
	Tcolor3<float> c = gluiPointLight.getColor();
	glPointSize(5.0);
	glColor3f(c.r, c.g, c.b);
	glBegin(GL_POINTS);
	glVertex3f(p.x, p.y, p.z);
	glEnd();
	glPointSize(1.0);
	//��̕`��
	skyDome->draw();
	skyDome->drawSunPosition(sun->getDirection(), cam->getViewpoint(), Tcolor3<float>(sun->getColor()*2.0));

	//Tvector3<float> p = pointLight->getPosition();
	//Tcolor3<float> c = pointLight->getColor();
	//glPointSize(3.0);
	//glColor3f(c.r, c.g, c.b);
	//glBegin(GL_POINTS);
	//glVertex3f(p.x, p.y, p.z);
	//glEnd();

    glutSwapBuffers ();
}
//----------------------------------------------------------------------
void TobjViewer::setGlutMenu() {
	vector<int> sub_menu_id;
	for(unsigned int i=0; i<glui_menu.size(); i++) {
		int sub_menu = glutCreateMenu(this->menu_operation);
		sub_menu_id.push_back(sub_menu);
		glui_menu.at(i)->createGlutMenu();
	}

	int main_menu = glutCreateMenu(this->menu_operation );
	for(unsigned int i=0; i<glui_menu.size(); i++) {
		glutAddSubMenu(menu_name.at(i).c_str(), sub_menu_id.at(i));
	}
	glutAddMenuEntry("quit", QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////�T�u���j���[�̓o�^�ƃ��C�����j���[�̐ݒ�
	////�J�����p�̃T�u���j���[�̐���
 //   int move_sub_menu = glutCreateMenu(this->menu_operation);
	//cam->setMenuEntry();

	//int main_menu = glutCreateMenu(menu_operation);
 //   glutAddSubMenu("view", move_sub_menu);
 //   glutAddMenuEntry("quit", QUIT);
 //   glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------
void TobjViewer::setGluiMenu() {
	GLUI *glui = GLUI_Master.create_glui("glui", 0, this->width+20, 0);
	GLUI_Master.set_glutIdleFunc(master->CallBackIdleFunc);

	GLUI_Panel *top_panel = glui->add_panel("");

	for(unsigned int i=0; i<glui_menu.size(); i++) {
		GLUI_Panel *sub_panel = glui->add_rollout_to_panel(top_panel, menu_name.at(i).c_str());
		glui_menu.at(i)->createGluiMenu(glui, sub_panel, menu_operation_glui);
		if(((i+1) % 3) == 0) glui->add_column_to_panel(top_panel);
	}


	gluiPointLight.createGluiMenu(glui, glui->add_rollout_to_panel(top_panel, "glui point light"), menu_operation_glui);

	//�I���{�^��
	glui->add_button("quit", QUIT, menu_operation_glui);

	glui->set_main_gfx_window(GetWindowID());
}
//----------------------------------------------------------------------
void TobjViewer::shadeObjects() {
	for(int i=0; i<objectManager->getNumObject(); i++) {
		Tobject *obj = objectManager->getObjectAddress(i);
		for(int j=0; j<obj->getNumVertex(); j++) {
			Tvertex *v = obj->getVertexAddress(j);
			v->setColor(Tcolor3<float>(0.0, 0.0, 0.0));
		}
	}

	objectManager->shadeObjects(*sun);

	for(int i=0; i<objectManager->getNumObject(); i++) {
		Tobject *obj = objectManager->getObjectAddress(i);
		Tcolor3<float> diffuse = obj->getMaterial().getDiffuse();
		for(int j=0; j<obj->getNumVertex(); j++) {
			Tvertex *v = obj->getVertexAddress(j);
			
			Tcolor3<float> cv = v->getColor();
			Tcolor3<float> c0 = pointLight->getIncidentLight(v->getPosition(), v->getNormal());
			Tcolor3<float> c1 = parallelLight->getIncidentLight(v->getPosition(), v->getNormal());

			v->setColor(Tcolor3<float>(cv + diffuse*(c0 + c1)));
			
		}
	}
}
