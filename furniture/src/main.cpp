#include "icg.h"
#include "constants.h"
#include "ogl.h"
#include "roomCalc.h"
#include "Furniture.h"
#include "myTexture.h"
#include "myLight.h"
#include "calcSade.h"
#include "calcAverageIlluminance.h"
#include "record.h"
#include<opencv2\opencv.hpp>

using namespace std;
//-------------------------------------
//�E�B���h�E�C�J�����C�}�E�X
ToglWindow		window;
Tmouse			mouse;
TglutCamera		cam;
TparallelLight	light;  //���s����
//TpointLight pointLight; //�_����
TobjectManager Wall, Floor, lamp; //�ǂƏ�
Furniture objects, minobjects, shadowobject; // , secondminobjects, thirdminobjects;

//�����̑傫��
GLfloat floor_scale = 350;
//X = blue, Y = green
GLfloat floorX, floorY;

//�t���O
int imode = -1;
int demoMode = OFF_90_DEGREE;

//�v�Z����
clock_t start;
clock_t end;

//�R�X�g�̏�����
float mincost = 9999,secondmin = 9999;

//MYLIGHT�N���X
vector<PointLight> ApointLight;
PointLight pointLight;
vector<ParallelLight> AparallelLight;
ParallelLight parallelLight;
vector<SpotLight> AspotLight,lampLight;
SpotLight spotLight;

//Table�쐬�̂��߂̂��낢��
vector<vector<float>> livingFluxTable;
vector<vector<float>> coffeeFluxTable;
float living_height;
float coffee_height;


//-------------------------------------

/* local functions */
static void display( void );
static void idle_func ( void );
void setFurniturePosition();
void setMoveGaussian();
void setMoveGaussian2();
void setRotationGaussian(void);
void setSwapRandom(void);
float densityFunction(float cost);
void Metropolis_Hastings(void);
float costFunction(Furniture& objects);
void recordTotext(Furniture& objects);
void recordmincost(clock_t start, clock_t end);
void drawFurniture(Furniture& minobjects);
void displayDrawDisk();
void setOnceReflectance(vector<Tvector3<float>>& ApointLight, vector<Tvector3<float>>& AspotLight);
//-------------------------------------------------------------
void loadobj()
{
	//-----text�t�@�C���ǂݍ��ݒ�----
	ifstream ifs[10];
	ifs[0]._open("text/armchair.txt");
	ifs[1]._open("text/sofa.txt");
	ifs[2]._open("text/coffee_table.txt");
	ifs[3]._open("text/armchair.txt");
	//ifs[4]._open("text/tv.txt");
	ifs[4]._open("text/living_table_small.txt");
	ifs[5]._open("text/living_chair.txt");
	ifs[6]._open("text/living_chair.txt");
	ifs[7]._open("text/tv.txt");
	//ifs[7]._open("text/living_chair.txt");
	//ifs[8]._open("text/living_chair.txt");
	//ifs[4]._open("text/bookshelf.txt");
	//ifs[7]._open("text/tv.txt");
	/*ifs[0]._open("text/living_table_small.txt");
	ifs[1]._open("text/living_chair.txt");
	ifs[2]._open("text/living_chair.txt");
	ifs[3]._open("text/living_chair2.txt");
	ifs[4]._open("text/living_chair2.txt");*/

	char* fname[objectNum];
	for (int i = 0; i < objectNum; i++){
		if (!ifs[i]){
			cerr << "text not opened ! " << endl;
			exit(1);
		}
		getline(ifs[i], objects.file[i]);
		int len = objects.file[i].length();
		fname[i] = new char[len + 1];
		memcpy(fname[i], objects.file[i].c_str(), len + 1);
		getline(ifs[i], objects.objname[i]);
		std::cout << objects.objname[i] << endl;
		objects.obj.readObject(fname[i]);

		objects.hopeRadius.push_back(-1);
		objects.hopeCenter.push_back(Tvector3<float>(0, 0, 0));
		objects.hopeScalex.push_back(1);
		objects.hopeScaley.push_back(1);

		minobjects.obj.readObject(fname[i]);
		minobjects.objname[i] = objects.objname[i];

		shadowobject.obj.readObject(fname[i]);
		shadowobject.objname[i] = objects.objname[i];
	}
	//--------------------------------
	//���ƕǂ̐ݒ�
	Floor.readObject("obj/floor17jou-2-mesh.obj");
	std::cout << "Floor" << endl;
	Floor[0].getMaterial().setDiffuse(FLOORCOLOR);
	Floor[0].invertVertexNormal();
	for (int i = 0; i < Floor[0].getNumVertex(); i++){
		Tvector3<float> p = Floor[0].getVertex(i).getPosition();
		Tvector3<float> n = Floor[0].getVertex(i).getNormal();
		swap(p.y, p.z);
		swap(n.y, n.z);
		Floor[0].getVertex(i).setPosition(p);
		Floor[0].getVertex(i).setNormal(n);
	}
	Floor[0].calBoundingBox();
	floorX = Floor[0].getMaximumPoint().x - Floor[0].getMinimumPoint().x;
	floorY = Floor[0].getMaximumPoint().y - Floor[0].getMinimumPoint().y;


	Wall.readObject("obj/wall17jou-2-mesh-mesh.obj");
	//Wall.readObject("obj/wall17jou-door-windows.obj");
	//Wall.readObject("obj/wall-door-windows-3.obj");
	std::cout << "Wall" << endl;
	Wall[0].getMaterial().setDiffuse(WALLCOLOR);
	Wall[0].invertVertexNormal();
	for (int i = 0; i < Wall[0].getNumVertex(); i++){
		Tvector3<float> p = Wall[0].getVertex(i).getPosition();
		Tvector3<float> n = Wall[0].getVertex(i).getNormal();
		swap(p.y, p.z);
		swap(n.y, n.z);
		Wall[0].getVertex(i).setPosition(p);
		Wall[0].getVertex(i).setNormal(n);
	}
	Wall[0].calBoundingBox();

	//�Ƌ�̓ǂݍ���
	for (int i = 0; i < objects.obj.getNumObject(); i++){
		objects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(0.81, 0.33, 0.41));
		objects.obj[i].invertVertexNormal();

		minobjects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(0.81, 0.33, 0.41));
		minobjects.obj[i].invertVertexNormal();

		if (objects.objname[i] == "coffee_table" || objects.objname[i] == "living_table"){
			objects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(0.48, 0.26, 0.20));
			minobjects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(0.48, 0.26, 0.20));
		}
		else if (objects.objname[i] == "living_chair"){
			objects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(28 / 256.0, 80 / 256.0, 147 / 256.0));
			minobjects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(28 / 256.0, 80 / 256.0, 147 / 256.0));
		}
		else if (objects.objname[i] == "tv"){
			objects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(184 / 256.0, 190 / 256.0, 191 / 256.0));
			minobjects.obj[i].getMaterial().setDiffuse(Tcolor3<float>(184 / 256.0, 190 / 256.0, 191 / 256.0));
		}
	}

	std::cout << "object�̐� = " << objects.obj.getNumObject() << endl;
	std::cout << "Floor x = " << floorX*1.4 << "   befor = " << floorX << endl;
	std::cout << "Floor y = " << floorY*1.4 << "   befor = " << floorY << endl;
	std::cout << "Wall z = " << (Wall[0].getMaximumPoint().z - Wall[0].getMinimumPoint().z)*1.5
		<< "   befor = " << (Wall[0].getMaximumPoint().z - Wall[0].getMinimumPoint().z) << endl;

	for (int j = 0; j < objects.obj.getNumObject(); j++){
		for (int i = 0; i < objects.obj[j].getNumVertex(); i++){
			Tvector3<float> p = objects.obj[j].getVertex(i).getPosition();
			Tvector3<float> n = objects.obj[j].getVertex(i).getNormal();
			swap(p.y, p.z);
			swap(n.y, n.z);
			objects.obj[j].getVertex(i).setPosition(p);
			objects.obj[j].getVertex(i).setNormal(n);

			Tvector3<float> pp = minobjects.obj[j].getVertex(i).getPosition();
			Tvector3<float> nn = minobjects.obj[j].getVertex(i).getNormal();
			swap(pp.y, pp.z);
			swap(nn.y, nn.z);
			minobjects.obj[j].getVertex(i).setPosition(pp);
			minobjects.obj[j].getVertex(i).setNormal(nn);

			Tvector3<float> ppp = shadowobject.obj[j].getVertex(i).getPosition();
			Tvector3<float> nnn = shadowobject.obj[j].getVertex(i).getNormal();
			swap(ppp.y, ppp.z);
			swap(nnn.y, nnn.z);
			shadowobject.obj[j].getVertex(i).setPosition(ppp);
			shadowobject.obj[j].getVertex(i).setNormal(nnn);
		}
		objects.obj[j].calBoundingBox();
		minobjects.obj[j].calBoundingBox();
		shadowobject.obj[j].calBoundingBox();
	}

	//�Ƌ�ƕ�����BoundingBox�̊m�F
	//std::cout << "------------------------------" << endl;
	//std::cout << "FLOOR = " << endl;
	//std::cout << "Max = " << Wall[0].getMaximumPoint() / 100.0 << endl;
	//std::cout << "Min = " << Wall[0].getMinimumPoint() / 100.0 << endl;
	//std::cout << "------------------------------" << endl;
	//for (int i = 0; i < objects.obj.getNumObject(); i++){
	//	std::cout << "Name = " << objects.objname[i] << endl;
	//	std::cout << "Max = " << objects.obj[i].getMaximumPoint() / 100.0 << endl;
	//	std::cout << "Min = " << objects.obj[i].getMinimumPoint() / 100.0<< endl;
	//	std::cout << "-----------------------------" << endl;
	//}
	//system("PAUSE");

	//��񔽎˂̂��߂̃e�X�g�Ɩ��݌v
	//--------------------�d�_�Ɩ����E����-----------------------------
	//�E�ɋ����o�[�W���� 190�{����
	float amb = 0.034; float kbri = 190;
	//int directivity = 1.0;			//�搶�Ƃ̊m�F�p
	//float bri0 = 1.9, bri1 = 1.6;		//�搶�Ƃ̊m�F�p
	//float cd0 = bri0 * kbri; float cd1 = bri1 * kbri;	//�搶�̊m�F�p

	Tvector3<float> direction1(0.0, 0.0, -1.0);
	Tvector3<float> direction2(0.0, 0.0, -1.0);
	Tcolor3<float> Lcolor = WHITE;
	
	////�E����:��������
	//Tvector3<float> position1(-225.0, 35.0, 240.0);
	//Tvector3<float> position2(-225.0, -35.0, 240.0);
	//Tvector3<float> position3(220.0, 0.0, 240.0);
	//int directivity = 2.0;
	//float cd0 = 360, cd1 = 450;
	//float bri0 = cd0 / kbri, bri1 = cd1 / kbri;
	////bri0 = 1.52632; bri1 = 2.42105;
	////cd0 = kbri*bri0; cd1 = kbri*bri1;
	
	////������(20000��ڂň����Ȃ�)
	//Tvector3<float> position1(225.0, 35.0, 240.0);
	//Tvector3<float> position2(225.0, -35.0, 240.0);
	//Tvector3<float> position3(-220.0, 0.0, 240.0);
	//int directivity = 2.0;
	//float cd0 = 360, cd1 = 450;
	//float bri0 = cd0 / kbri, bri1 = cd1 / kbri;	

	//�����߁F��������(10000��ȓ�)
	Tvector3<float> position1(225.0, 35.0, 240.0);
	Tvector3<float> position2(225.0, -35.0, 240.0);
	Tvector3<float> position3(-220.0, 0.0, 240.0);
	int directivity = 2.0;
	float cd0 = 340, cd1 = 470;
	float bri0 = cd0 / kbri, bri1 = cd1 / kbri;	

	////���S�Ɍ������W�߂��Ƃ�
	//Tvector3<float> position1(0.0, 20.0, 240.0);
	//Tvector3<float> position2(0.0, -20.0, 240.0);
	//Tvector3<float> position3(220.0, 0.0, 240.0);
	//int directivity = 2.0;
	////float cd0 = 1000, cd1 = 0;
	//float cd0 = 380, cd1 = 0;
	//float bri0 = cd0 / kbri, bri1 = cd1 / kbri;

	////�΂����Ɩ��z�u�̂Ƃ� 
	//Tvector3<float> position1(-250, 80, 240);
	//Tvector3<float> position2(-180, 0.0, 240);
	//Tvector3<float> position3(-250, -80, 240);
	//int directivity = 1.0;
	//float cd0 = 530, cd1 = 530;
	//float bri0 = cd0 / kbri, bri1 = cd1 / kbri;

	spotLight.setAllParam(position1.x, position1.y, position1.z, Lcolor, bri0, cd0, direction1.x, direction1.y, direction1.z, amb, directivity);
	AspotLight.push_back(spotLight);
	spotLight.setAllParam(position2.x, position2.y, position2.z, Lcolor, bri0, cd0, direction1.x, direction1.y, direction1.z, amb, directivity);
	AspotLight.push_back(spotLight);
	spotLight.setAllParam(position3.x, position3.y, position3.z, Lcolor, bri1, cd1, direction2.x, direction2.y, direction2.z, amb, directivity);
	AspotLight.push_back(spotLight);

	//---------------�x�[�X�Ɩ�(60�`)(PointLight)----------------------
	//float bri2 = 0.6;		//�搶�Ƃ̊m�F�p
	//float cd2 = bri2 * kbri;//�搶�̊m�F�p
	
	//�x�[�X�Ɩ�(���E�Ƃ�����)
	Tvector3<float> baseposition(280.0, 170.0, 240.0);
	float cd2 = 120;
	float bri2 = cd2 / kbri;
	//bri2 = 0.578947;
	//cd2 = kbri*bri2;

	////���S�Ɍ������W�߂��Ƃ�
	//Tvector3<float> baseposition(280.0, 170.0, 240.0);
	//float cd2 = 20;
	//float bri2 = cd2 / kbri;

	////�΂����͏Ɩ��z�u�̂Ƃ�
	//cd2 = 20.0;	bri2 = cd2 / kbri;


	pointLight.setAllParam(300.0, 180.0, 240.0, Lcolor, bri2, amb, cd2);
	ApointLight.push_back(pointLight);
	pointLight.setAllParam(300.0, -180.0, 240.0, Lcolor, bri2, amb, cd2);
	ApointLight.push_back(pointLight);
	pointLight.setAllParam(-300.0, 180.0, 240.0, Lcolor, bri2, amb, cd2);
	ApointLight.push_back(pointLight);
	pointLight.setAllParam(-300.0, -180.0, 240.0, Lcolor, bri2, amb, cd2);
	ApointLight.push_back(pointLight);
	pointLight.setAllParam(0.0, 0.0, 240.0, Lcolor, bri2, amb, cd2);
	ApointLight.push_back(pointLight);
	//---------------------------------------------------------------------
	//��������txt�ɂ���
	lightRecord("datafile/light/light.txt", ApointLight, AspotLight);
	//system("PAUSE");
	//---------------------------------------------------------------------
	//���ݔ���(1�񔽎˂�����)�˕ǂ̃��b�V�����l���ē_������z�u�Cpushback����
	/* �K�v�Ȉ����ꗗ
	void calcOnceRefraction(
	vector<Tvector3<float>>& wallpoint ���ǂ�4���_,
	vector<PointLight>& pointLight, vector<SpotLight>& spotLight, �����Ɩ��Ƃ��Ďg���Ă�pointLight��spotLight
	int pnum ���Ɩ�pointLight�̐�, int snum ���Ɩ�spotLight�̐�,
	int imesh �����̃��b�V���̐�, int jmesh ���c�̃��b�V���̐�,
	float reflectance �����˗�, Tvector3<float> normal ���ǂ̖@��, Tcolor3<float>& color ���Ɩ��̐F)
	*/
	vector<Tvector3<float>> wallpoint; wallpoint.resize(4);
	float reflectance = (WALLCOLOR.r + WALLCOLOR.g + WALLCOLOR.b) / 3.0;
	int pnum = ApointLight.size(), snum = AspotLight.size();
	std::cout << "befor ApointLight.size() = " << pnum << endl;
	std::cout << "befor AspotLight.size() = " << snum << endl;
	Tvector3<float> wallnormal;

	bool refon = TRUE;
	if (refon==TRUE){
		//�E�E�E�E�E�E�E��(ceiling)���Ƃɐݒ肷��E�E�E�E�E�E�E�E�E�E�E
		wallpoint[0] = Tvector3<float>(523.822, -350.308, 259.588);
		wallpoint[1] = Tvector3<float>(-523.822, -350.308, 259.588);
		wallpoint[2] = Tvector3<float>(-523.822, 350.308, 259.588);
		wallpoint[3] = Tvector3<float>(523.822, 350.308, 259.588);
		wallnormal = Tvector3<float>(0.0, 0.0, -1.0);
		int imesh = 3; int jmesh = 5;
		//imesh = 10; jmesh = 20;
		//�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E
		calcOnceRefraction(wallpoint, ApointLight, AspotLight, pnum, snum, imesh, jmesh, reflectance, wallnormal, Lcolor);
		cout << "After ApointLight.size() = " << ApointLight.size() << endl << "�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E" << endl;

		//�ǂ͔��ˌ���shade�����Ȃ��D
		calcAllLightIntensity_wall(ApointLight, AparallelLight, AspotLight, Wall[0]);


		//�E�E�E�E�E�E�E��(left)���Ƃɐݒ肷��E�E�E�E�E�E�E�E�E�E�E
		wallpoint[0] = Tvector3<float>(523.822, 350.308, 259.588);
		wallpoint[1] = Tvector3<float>(523.822, -350.308, 259.588);
		wallpoint[2] = Tvector3<float>(523.822, -350.308, 0.0);
		wallpoint[3] = Tvector3<float>(523.822, 350.308, 0.0);
		wallnormal = Tvector3<float>(-1.0, 0.0, 0.0);
		imesh = 3; jmesh = 1;
		//imesh = 20; jmesh = 10;
		//�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E
		calcOnceRefraction(wallpoint, ApointLight, AspotLight, pnum, snum, imesh, jmesh, reflectance, wallnormal, Lcolor);
		std::cout << "After ApointLight.size() = " << ApointLight.size() << endl << "�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E" << endl;

		//�E�E�E�E�E�E�E��(back)���Ƃɐݒ肷��E�E�E�E�E�E�E�E�E�E�E
		wallpoint[0] = Tvector3<float>(523.822, -350.308, 259.588);
		wallpoint[1] = Tvector3<float>(-523.822, -350.308, 259.588);
		wallpoint[2] = Tvector3<float>(-523.822, -350.308, 0.0);
		wallpoint[3] = Tvector3<float>(523.822, -350.308, 0.0);
		wallnormal = Tvector3<float>(0.0, 1.0, 0.0);
		imesh = 5; jmesh = 1;
		//imesh = 20; jmesh = 10;
		//�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E
		calcOnceRefraction(wallpoint, ApointLight, AspotLight, pnum, snum, imesh, jmesh, reflectance, wallnormal, Lcolor);
		std::cout << "After ApointLight.size() = " << ApointLight.size() << endl << "�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E" << endl;

		//�E�E�E�E�E�E�E��(right)���Ƃɐݒ肷��E�E�E�E�E�E�E�E�E�E�E
		wallpoint[0] = Tvector3<float>(-523.822, -350.308, 259.588);
		wallpoint[1] = Tvector3<float>(-523.822, 350.308, 259.588);
		wallpoint[2] = Tvector3<float>(-523.822, 350.308, 0.0);
		wallpoint[3] = Tvector3<float>(-523.822, -350.308, 0.0);
		wallnormal = Tvector3<float>(1.0, 0.0, 0.0);
		imesh = 3; jmesh = 1;
		//imesh = 20; jmesh = 10;
		//�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E
		calcOnceRefraction(wallpoint, ApointLight, AspotLight, pnum, snum, imesh, jmesh, reflectance, wallnormal, Lcolor);
		std::cout << "After ApointLight.size() = " << ApointLight.size() << endl << "�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E" << endl;

		//�E�E�E�E�E�E�E��(this side)���Ƃɐݒ肷��E�E�E�E�E�E�E�E�E�E�E
		wallpoint[0] = Tvector3<float>(-523.822, 350.308, 259.588);
		wallpoint[1] = Tvector3<float>(523.822, 350.308, 259.588);
		wallpoint[2] = Tvector3<float>(523.822, 350.308, 0.0);
		wallpoint[3] = Tvector3<float>(-523.822, 350.308, 0.0);
		wallnormal = Tvector3<float>(0.0, -1.0, 0.0);
		imesh = 5; jmesh = 1;
		//imesh = 20; jmesh = 10;
		//�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E
		calcOnceRefraction(wallpoint, ApointLight, AspotLight, pnum, snum, imesh, jmesh, reflectance, wallnormal, Lcolor);
		std::cout << "After ApointLight.size() = " << ApointLight.size() << endl << "�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E" << endl;
	}

	//system("PAUSE");

	//LightIntensity�̂Ƃ���FLOOR��WALL�Ȃ狭�x����߂�悤�ɔ��������Ȃ��Ƃ����Ȃ������B�B�B
	//�Ȃ��Ȃ�ǂ����₯�ɖ��邷���邩��i���W�ݒ�ŏI����Ă܂��������j
	calcAllLightIntensity_floor(ApointLight, AparallelLight, AspotLight, Floor[0]);
	//calcAllLightIntensity_wall(ApointLight, AparallelLight, AspotLight, Wall[0]);

	
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void initScene()
{
	loadobj();

	Tvector3<float> refe = Tvector3<float>(0, 2.6, 1.5);
	Tvector3<float> view = refe + Tvector3<float>(0, 700, 700);

	//Tvector3<float> refe = Tvector3<float>(23.187, 512.626, 403.49);
	//Tvector3<float> view = Tvector3<float>(24.55, -272.75, -199.144);

	cam.init(view,
		refe,
		1.0, 500000.0, TO_RAD(60.0), 0.0,
		window.getWindowSize());
	cam.setMoveScale(1.);

}
//----------------------------------------------------------------------
void quit() {
	exit(1);
}
//----------------------------------------------------------------------
static void key_func ( unsigned char key, int x, int y )
{
	if(cam.keyFunc(key, x, y)) imode = CAMERA_MODE;

	if(key == KEY_ESC) quit();
	if (key == '/') {
		int flag = rand() % 3;
		if (flag == 0){
			setMoveGaussian();
		}
		else if (flag == 1){
			setRotationGaussian();
		}
		else if (flag == 2){
			setSwapRandom();
		}
	}
	glutPostRedisplay();
}
//----�����ɉ~��`�悷��---------------------------------------------------
GLUquadric *qobj;
void drawDisk(Tvector3<float> p, GLdouble inner, GLdouble outer, GLfloat scalex = 1.0, GLfloat scaley = 1.0)
{
	qobj = gluNewQuadric();
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(p.x, p.y, p.z);
	glScalef(scalex, scaley, 1.0);
	glTranslatef(0.0, 0.0, 0.1);
	gluDisk(qobj, inner, outer, 150, 250);
	glPopMatrix();
	gluDeleteQuadric(qobj);
}


//----3�����ʒu���N���b�N�Ŏ擾--------------------------------------------
Tvector3<float> getFloorPosition(int x, int y)
{
	Tvector3<float> p = cam.invProjection(Tvector2<float>(x, window.getHeight() - y));
	Tvector3<float>  view = cam.getViewpoint();
	Tvector3<float> n = Floor[0].getVertex(0).getNormal();

	float t = dot(n, (-view)) / dot(n, p - view);

	Tvector3<float> q = view + t*(p - view);
	return q;
}

// ----------------------------------------------------------------------
Tvector3<float> pick_point;
GLdouble radius;
string choiceFname;
int pick_i;
int hope_flag = 0;
void mouse_func ( int button, int state, int x, int y )
{
	mouse.updateStatusGL(button, state, x, y);
	if (mouse.isButtonDown(0)){
		Tvector3<float> q = getFloorPosition(x, y);
		pick_point = q;
		std::cout << q << endl;
		if (imode == PICK_MODE){
			for (int i = 0; i < objectNum; i++){
				if ((q.x - minobjects.trans[i].x)*(q.x - minobjects.trans[i].x)
					+ (q.y - minobjects.trans[i].y)*(q.y - minobjects.trans[i].y)
					<= minobjects.obj[i].getBoundingSphereRadius()*minobjects.obj[i].getBoundingSphereRadius()
					)
				{
					choiceFname = minobjects.objname[i];
					pick_i = i;
					std::cout << "�Ƌ�@���@" << minobjects.objname[i] << endl;
					std::cout << "�ԍ��@���@" << pick_i << endl;
				}
			}
		}
		if (imode == HOPE_MODE){
			objects.hopeCenter[pick_i] = pick_point;
			objects.hopeScalex[pick_i] = 1.5;
			objects.hopeScaley[pick_i] = 0.7;
		}
		if (imode == TABLE_DEMO){
			mincost = INT_MAX;
			hope_flag = 1;
			objects.hopeCenter[4] = pick_point;
			objects.hopeRadius[4] = 150;
		}

	}

}
//----------------------------------------------------------------------
Tvector3<float> p;
void motion_func ( int x, int y )
{
	mouse.setCurrentPosition(x, y);

	if(imode == CAMERA_MODE) 
		cam.motionFunc(mouse.getCurPos(), mouse.getMotionVec());

	mouse.setPreviousPosition(x, y);

	if (mouse.isDragging()){
		p = getFloorPosition(x, y);
		radius = (p - pick_point).length();
		if (imode == HOPE_MODE){
			objects.hopeRadius[pick_i] = radius;
			std::cout << "���a�@=�@" << radius << endl;
		}
		if (imode == TABLE_DEMO){
			mincost = INT_MAX;
			hope_flag = 1;
			objects.hopeCenter[4] = p;
			objects.hopeRadius[4] = 150;
			//drawDisk(p, 145, 150);
		}
	}

    glutPostRedisplay();
}

//----------------------------------------------------------------------
//��ɌĂ΂��
vector<Tvector3<float>> mintrans,secondmintrans,thirdmintrans,fourthmintrans,fifthmintrans,sixthmintrans,seventhmintrans;
vector<float> minzrot, secondminzrot, thirdminzrot,fourthminzrot,fifthminzrot,sixthminzrot,seventhminzrot;
static int loopCount = 0,subloop = 0;
int boxnum = 0;
static void idle_func ( void )
{
	for (int i = 0; i < 1000; i++){
		Metropolis_Hastings();
		////--�����_���ɔz�u�������ʂ�txt�ɂ���֐���p��--
		//if (subloop % 333 == 0){
		//	char buff[10]; itoa(boxnum, buff, 10); char s1[256] = "datafile/box/box"; char *s2 = ".txt";
		//	strcat(s1, buff); strcat(s1, s2);
		//	recordBox(s1,Wall,objects); boxnum++;
		//	cout << "subloop = " << subloop << " , boxnum = " << boxnum << endl;
		//}
		////-----------------------------------------------
		loopCount++;
		if (loopCount % 100 == 0){
			std::cout << loopCount << endl;
		}
		subloop++;
	}


	if (loopCount > 49000){
		glutIdleFunc(NULL);
	}
	
	if (subloop > 9000){
	//if (subloop > 12000){
		glutIdleFunc(NULL);
		mincost = INT_MAX;
	}
	glutPostRedisplay();	
}

//-----���g���|���X�@�w�C�X�e�B���O�X----------------------------
void Metropolis_Hastings(void)
{
	float p0, cost, p, alpha;
	vector<Tvector3<float>> temptrans = objects.trans;
	vector<float> tempzrot = objects.zrot;

	cost = costFunction(objects);
	p0 = densityFunction(cost);
	int flag = rand() % 3;
	if (flag == 0){
		setMoveGaussian();
	}
	else if (flag == 1){
		setRotationGaussian();
	}
	else if (flag == 2){
		setSwapRandom();
	}

	cost = costFunction(objects);
	p = densityFunction(cost);
	alpha = p / p0;
	if (alpha > 1) alpha = 1.0;
	float t = ((float)rand() + 1.0) / ((float)RAND_MAX + 2.0);
	if (alpha <= t){
		objects.trans = temptrans;
		objects.zrot = tempzrot;
	}


	if (mincost > cost){
		//2�ԖځA3�ԖځA4�ԖځA5�Ԗڂɏ������R�X�g��ۑ�
		seventhmintrans = sixthmintrans;
		seventhminzrot = sixthminzrot;
		
		sixthmintrans = fifthmintrans;
		sixthminzrot = fifthminzrot;

		fifthmintrans = fourthmintrans;
		fifthminzrot = fourthminzrot;
		
		fourthmintrans = thirdmintrans;
		fourthminzrot = thirdminzrot;

		thirdmintrans = secondmintrans;
		thirdminzrot = secondminzrot;

		secondmin = mincost;
		secondmintrans = mintrans;
		secondminzrot = minzrot;

		mincost = cost;
		std::cout << "loopCount = " << loopCount << endl;
		std::cout << "mincost = " << mincost << endl;

		////------authoring--------
		//authoring_befor(objects);
		////-----------------------

		mintrans = objects.trans;
		minzrot = objects.zrot;
		
		clock_t end = clock();
		//------------------------mincost_record_���낢��------------------------------
		recordmincost(start, end);
		//-----------------------------------------------------------------------------
	}


}

//----���W�����o���Ă݂�---------------------------------------------------
void DispXYZAxes(void)
{
	glBegin(GL_LINES);
	glColor3f(0, 0, 1); glVertex3i(0, 0, 0); glVertex3i(1000, 0, 0); // X��(blue)
	glColor3f(0, 1, 0); glVertex3i(0, 0, 0); glVertex3i(0, 1000, 0); // Y��(green)
	glColor3f(1, 0, 0); glVertex3i(0, 0, 0); glVertex3i(0, 0, 1000); // Z��(red)
	glEnd();
}
//-----------------------------------------------------------------------
//�d�Ȃ��Ă�Ƃ�TRUE�@�d�Ȃ��ĂȂ��Ƃ�FALSE
bool checkOverlap(int i, int j, float tx, float ty){
	Tvector3<float> center_i = objects.obj[i].getCenter();
	Tvector3<float> center_j = objects.obj[j].getCenter();
	center_i.z = 0.0;
	center_j.z = 0.0;
	Tvector3<float> r = (center_i + Tvector3<float>(tx, ty, 0)) - (center_j + objects.trans[j]);
	float sphereRadius_i = objects.obj[i].getBoundingSphereRadius();
	float sphereRadius_j = objects.obj[j].getBoundingSphereRadius();
	float side_i = objects.obj[i].getMaximumPoint().z - objects.obj[i].getCenter().z;
	float side_j = objects.obj[j].getMaximumPoint().z - objects.obj[j].getCenter().z;
	float rri = sqrtf((sphereRadius_i*sphereRadius_i) - (side_i*side_i));
	float rrj = sqrtf((sphereRadius_j*sphereRadius_j) - (side_j*side_j));
	float rr = rri + rrj;
	if (r.length() < rr){
		return TRUE;
	}
	return FALSE;
}
//-----------------------------------------------------------------------
bool checkClipping(void)
{
	return false;
}

//-----------------------------------------------------------------------
void setFurniturePosition(){
	float tx, ty, r_z;
	objects.trans.clear();
	objects.zrot.clear();
	for (int i = 0; i < objectNum; i++){
		bool flag = TRUE;
		while (flag == TRUE){ 
			tx = ((float)rand() / RAND_MAX) * (floorX)-(floorX / 2.0);
			ty = ((float)rand() / RAND_MAX) * (floorY)-(floorY / 2.0);
			
			//r_z = (float)rand() / RAND_MAX + rand() % 180;
			flag = FALSE;
			for (int j = 0; j < i; j++){
				flag = checkOverlap(i, j, tx, ty);
				if (flag == TRUE) break;
			}
		}
		objects.trans.push_back(Tvector3<float>(tx, ty, 0.0));
		
		////��]�p��90�x�Ԋu��
		//int rotflag = rand() % 4;
		//if (rotflag == 0) r_z = 0;
		//else if (rotflag == 1) r_z = 90;
		//else if (rotflag == 2) r_z = 180;
		//else if (rotflag == 3) r_z = 270;

		//��]�p��A���l
		r_z = (float)rand() / RAND_MAX + rand() % 180;
		
		
		objects.zrot.push_back(r_z);
	}
}

float costFunction(Furniture& objects)
{
	float cost, mcv, mpd, mpa, mcd, mca, mvb, mfa, mwa, mef;
	float mhd; // �V������]���z
	float mld; // �Ɩ����l��������
	//mcv = calcClearance(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	//mcv = calcClearance(floor_scale, floor_scale);
	calcPairRelation(objects, mpd, mpa);
	calcConversation(objects, mcd, mca);
	mvb = calcVisualBalance(objects);
	calcAlignment(objects, mfa, mwa);
	mef = calcEmphasis(objects);

	mhd = calcHopeDistribution3(objects);

	//�O���[�X�P�[���ɂ������̏Ɩ����z
	//mld = calcLightDistribution(objects, ApointLight, AspotLight);
	//1�_�����̏Ɠx��p�����]���֐�
	//mld = calcIlluminanceDistribution(objects, ApointLight);
	
	//���ϏƓx��p�����]���֐�
	mld = calcAverageIlluminanceDistribution(livingFluxTable, coffeeFluxTable, living_height, coffee_height, 4, 2, objects);

	float wcv = 2.0, wci = 1.0, wpd = 2.0, wpa = 2.0, wcd = 2.0, wca = 2.0, wvb = /*1.5*/0.7, wfa = 2.5, wwa = 2.5, wsy = 1.0, wef = 4.0;

	//��]���z�̂Ƃ��̏d��
	float whd = 7.0;
	//float whd = 5.0;
	//float whd = 2.5;

	//�Ɩ����z�̏d��
	//float wld = 7.5;
	float wld = 8.0;

	cost = /*wcv*mcv*/ + wpd*mpd + wpa*mpa + wcd*mcd + wca*mca + wvb*mvb + wfa*mfa + wwa*mwa;

	//��]���z������Ƃ�
	//cost += whd*mhd;

	//�R�X�g���ɏƖ��݌v������Ƃ�
	cost += wld*mld;

	return cost;
}

void setMoveGaussian(){
	int furnitureID = rand() % objectNum;
	float sigma = 50;
	bool flag = TRUE;

	while (flag == TRUE){
		float tx = rand_normal(0, sigma);
		float ty = rand_normal(0, sigma);
		objects.trans[furnitureID].z = 0;

		while (objects.trans[furnitureID].x + tx> floorX/2.0 || objects.trans[furnitureID].y +ty> floorY/2.0 
			   || objects.trans[furnitureID].x +tx< -floorX/2.0 || objects.trans[furnitureID].y +ty< -floorY/2.0){
			tx = rand_normal(0, sigma);
			ty = rand_normal(0, sigma);
		}

		objects.trans[furnitureID].x += tx;
		objects.trans[furnitureID].y += ty;

		flag = FALSE;
		for (int j = 0; j < objectNum; j++){
			if (j == furnitureID) j++;
			flag = checkOverlap(furnitureID, j, objects.trans[furnitureID].x, objects.trans[furnitureID].y);
			if (flag == TRUE) break;
		}
	}
}


void setRotationGaussian(void){
	int furnitureID = rand() % objectNum;
	

	//�A���̒l�ŉ�]����悤�ɂ������� sigma =180 �ɂ���Ƃ��������������
	//float sigma = 10; 
	float sigma = 90;
	objects.zrot[furnitureID] += rand_normal(0, sigma);

	////90�x�Ԋu�ŗ��U�l�ŉ�]����
	//float sigma = 90;
	//int rot;
	//float rand = rand_normal(0, sigma);
	//if (rand < 0){
	//	rand = -rand;
	//	rot = (int)rand % 360;
	//	rot = -rot;
	//}
	//else{
	//	rot = (int)rand % 360;
	//}
	//if (rot > -45 && rot <= 45) rot = 0;
	//else if (rot > 45 && rot <= 135) rot = 90;
	//else if (rot > 135 && rot <= 225) rot = 180;
	//else if (rot > 225 && rot <= 315) rot = 270;
	//else if (rot > 315 && rot <= 405) rot = 360;
	//else if (rot > -135 && rot <= -45) rot = -90;
	//else if (rot > -225 && rot <= -135) rot = -180;
	//else if (rot > -315 && rot <= -225) rot = -270;
	//else if (rot > -405 && rot <= -315) rot = -360;
	//objects.zrot[furnitureID] += rot;

}

void setSwapRandom(void){
	int one_furnitureID, two_furnitureID;
	one_furnitureID = rand() % objectNum;
	two_furnitureID = one_furnitureID;
	while (two_furnitureID == one_furnitureID){
		two_furnitureID = rand() % objectNum;
	}
	Tvector3<float> temp;
	float ztemp;
	temp = objects.trans[one_furnitureID];
	ztemp = objects.zrot[one_furnitureID];

	objects.trans[one_furnitureID] = objects.trans[two_furnitureID];
	objects.zrot[one_furnitureID] = objects.zrot[two_furnitureID];

	objects.trans[two_furnitureID] = temp;
	objects.zrot[two_furnitureID] = ztemp;

}
//-------------------------------------------------------------------
GLfloat lx = 0.5, ly = 1.0, lz = 1.0;
GLfloat shadowMat[16] = {
	lz, 0, 0, 0,
	0, lz, 0, 0,
	-lx, -ly, 0, 0,
	0, 0, 0, lz,
};

static void display( void )
{
	//�Ƌ�\���v���O����
	glutSetWindow(window.getWindowID());
	window.clear();

	//cam.setAspectRatio(525 / 350.0);
	cam.glCamera();

	glViewport(0.0, 0.0, window.getWidth()/1.5, window.getHeight());

	//swap�����܂�
	vector<Tvector3<float>> temptrans;
	vector<float> tempzrot;
	if (imode == SWAP_1_2){
		temptrans = mintrans;
		tempzrot = minzrot;
		mintrans = fifthmintrans;
		minzrot = fifthminzrot;
		fifthmintrans = temptrans;
		fifthminzrot = tempzrot;
		imode = CAMERA_MODE;
	}
	else if (imode == SWAP_1_3){
		temptrans = mintrans;
		tempzrot = minzrot;
		mintrans = seventhmintrans;
		minzrot = seventhminzrot;
		seventhmintrans = temptrans;
		seventhminzrot = tempzrot;
		imode = CAMERA_MODE;
	}


	//======================minobjects��\��=====================================
	////�E�����ɕ\������@�R�X�g�ŏ��̂��̂��o��������
	minobjects.trans = mintrans;
	minobjects.zrot = minzrot;

	//�Ƌ�A�ǁA����\��
	drawFurniture(minobjects);
	std::cout << "���r���O�e�[�u���̏Ɠx(min)�@���@" << calcAverageIlluminance(livingFluxTable, living_height, 4, minobjects) << endl;
	std::cout << "�R�[�q�[�e�[�u���̏Ɠx(min)�@���@" << calcAverageIlluminance(coffeeFluxTable, coffee_height, 2, minobjects) << endl;
	//testIlluminance(ApointLight, minobjects);

	//���W��\�����Ă܂�
	//DispXYZAxes();
	//�~��`��
	//displayDrawDisk();

	//�œK����������box��ۑ�
	recordBox("datafile/optbox/optbox.txt",Wall,minobjects);
	//text�ɕۑ�"layout/layout1.txt"�ŕۑ�
	recordFurnitureLayout("datafile/layoutText/layout1.txt", minobjects);


	//================����minobjects��\��===============================================

	//cam.setAspectRatio(262.0 / 175.0);
	//cam.glCamera();

	glViewport(window.getWidth() / 1.5, window.getHeight() / 2.0, window.getWidth() / 3.0, window.getHeight() / 2.0);

	minobjects.trans = fifthmintrans;
	minobjects.zrot = fifthminzrot;

	drawFurniture(minobjects);
	std::cout << "���r���O�e�[�u���̏Ɠx(second)�@���@" << calcAverageIlluminance(livingFluxTable, living_height, 4, minobjects) << endl;
	std::cout << "�R�[�q�[�e�[�u���̏Ɠx(second)�@���@" << calcAverageIlluminance(coffeeFluxTable, coffee_height, 2, minobjects) << endl;
	
	//text�ɕۑ�"layout/layout2.txt"�ŕۑ�
	recordFurnitureLayout("datafile/layoutText/layout2.txt", minobjects);

	//================thirdminobjects��\��=================================================
	glViewport(window.getWidth() / 1.5, 0.0, window.getWidth() / 3.0, window.getHeight() / 2.0);

	minobjects.trans = seventhmintrans;
	minobjects.zrot = seventhminzrot;

	drawFurniture(minobjects);
	std::cout << "���r���O�e�[�u���̏Ɠx(third)�@���@" << calcAverageIlluminance(livingFluxTable, living_height, 4, minobjects) << endl;
	std::cout << "�R�[�q�[�e�[�u���̏Ɠx(third)�@���@" << calcAverageIlluminance(coffeeFluxTable, coffee_height, 2, minobjects) << endl;
	std::cout << "-----------------------------" << endl;

	//text�ɕۑ�"laout/layout3.txt"�ŕۑ�
	recordFurnitureLayout("datafile/layoutText/layout3.txt", minobjects);

	glutSwapBuffers ();

}
//----------------------------------------------------------------------
float densityFunction(float cost)
{
	//float beta = 0.55; //�b���� whd = 2.5
	//float beta = 0.7; //������� 20�����[�v�̂Ƃ� whd ���ꂽ�Ƃ���3�ʂɂȂ��
	//float beta = 0.8; //������ʁ@20�����[�v�̂Ƃ� whd ���ꂽ�Ƃ���2��
	
	//float beta = 0.85; //whd���ꂽ�Ƃ��͈��|�I1�� 

	//-----------[]�Ȃ�--------------------------------------------
	//float beta = 0.75; // �� i=635 0.074s
	//float beta = 0.74; //�� i=635 
	//float beta = 0.71; //�����@������i = 16330 0.44s�@������ƕς�� but []�Ȃ�
	//float beta = 0.715; //�`���@i=198512 4.5s
	//float beta = 0.705;
	//float beta = 0.95;

	//-------------[]����----------------------------------------
	//float beta = 0.85; //�����@������i=3����@0.8s but []����
	//float beta = 0.86;	//��
	//float beta = 0.855;	//����
	float beta = 0.85;


	return exp2f(-beta*cost);
}

//----------------------------------------------------------------------

void menu_operation(int val) {

	if (val == QUIT) quit();
	if (val == START) { start = clock(); glutIdleFunc(idle_func); }
	if (val == STOP) glutIdleFunc(NULL);
	if (val == PICK_MODE){ imode = val; }
	if (val == HOPE_MODE){ imode = val; }
	if (val == SUGGEST)	{ imode = val; glutIdleFunc(idle_func); subloop = 0; }

}
//----------------------------------------------------------------------
void cam_menu_operation(int val)
{
	imode = CAMERA_MODE;
	cam.menuOperation(val);
}
//-----------------------------------------------------------------------
void submenu_operation(int val)
{
	if (val == TABLE_DEMO){	imode = val; }
	if (val == ON_90_DEGREE){ demoMode = val; }
	if (val == OFF_90_DEGREE){ demoMode = val; }
}

void swap_operation(int val)
{
	if (val == SWAP_1_2){ imode = val; }
	if (val == SWAP_1_3){ imode = val; }
}


//----------------------------------------------------------------------
void setGlutMenu() {
	
	int demo_submenu = glutCreateMenu(submenu_operation);
	glutAddMenuEntry("table_demo", TABLE_DEMO);
	glutAddMenuEntry("ON_90degree_demo", ON_90_DEGREE);
	glutAddMenuEntry("OFF_90degree_demo", OFF_90_DEGREE);

	int swap_menu = glutCreateMenu(swap_operation);
	glutAddMenuEntry("1��2", SWAP_1_2);
	glutAddMenuEntry("1��3", SWAP_1_3);

	int cam_menu = cam.createGlutMenu(cam_menu_operation);

    int main_menu = glutCreateMenu(menu_operation);
	glutAddSubMenu("camera", cam_menu);
	glutAddMenuEntry("pick mode", PICK_MODE);
	glutAddMenuEntry("hope mode", HOPE_MODE);
	glutAddSubMenu("demo_subtest", demo_submenu);
	glutAddSubMenu("Swap", swap_menu);
	glutAddMenuEntry("start", START);
	glutAddMenuEntry("suggestion", SUGGEST);
	glutAddMenuEntry("stop", STOP);
	glutAddMenuEntry("quit", QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------
void initWindow(int w, int h) {
	//---------------------------------------------
	//�E�B���h�E�̏�����
	window.setSize(w, h);
	window.setTitle("window1");
	window.setClearColor(CLEAR_COLOR);
	window.create();
	
}
//----------------------------------------------------------------------
//table���ł��Ă��邩�m�F����
void saveImage(vector<float>& table)
{
	Timage image(mesh_size, mesh_size, 3);
	Tcolor4<unsigned char> c;
	for (int i = 0; i < mesh_size; i++){
		for (int j = 0; j < mesh_size; j++){
			c.r = (unsigned char)CLAMP(0, (table[i*mesh_size + j] / table[mesh_size*mesh_size - 1]) * 255, 255);
			c.g = c.b = c.r;
			image.set(i, j, c);
		}
	}
	image.save("test.bmp");
	std::cout << "save�����ł�" << endl;
}

void initTable()
{
	////�c�Ɖ��̃��b�V���ׂ̍���(���߂�100)
	//#define mesh_size 15
	////��]�̃��b�V���ׂ̍���(���߂�100)
	//#define zrot_size 15
	clock_t startTable = clock();
	//livingTable�̍����Acoffee_table�̍���
	living_height = (objects.obj[4].getMaximumPoint().z - objects.obj[4].getMinimumPoint().z) / 100.0;
	coffee_height = (objects.obj[2].getMaximumPoint().z - objects.obj[2].getMinimumPoint().z) / 100.0;
	makeLightFluxTable(livingFluxTable, living_height, ApointLight, AspotLight);
	clock_t endTable = clock();
	std::cout << "diningTable time = " << (double)(endTable - startTable) / CLOCKS_PER_SEC << " [s]"<< endl;
	startTable = clock();
	makeLightFluxTable(coffeeFluxTable, coffee_height, ApointLight, AspotLight);
	endTable = clock();
	std::cout << "coffeeTable time = " << (double)(endTable - startTable) / CLOCKS_PER_SEC << " [s]"<< endl;

	saveImage(livingFluxTable[0]);
	std::cout << "���̕��ϏƓx�@���@" << livingFluxTable[0][mesh_size*mesh_size - 1]/124.08 << endl;

}

//----------------------------------------------------------------------
void initGlut()
{

	//---------------------------------------------
	//���j���[�̏�����
	setGlutMenu();
    
    glutKeyboardFunc ( key_func );
    glutMouseFunc ( mouse_func );
    glutMotionFunc ( motion_func );	
    glutDisplayFunc ( display );
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	////---------------------------------------------
	//int a;
	//cout << "-------------------------------------" << endl;
	//cout << "Select DegreeMode (0:off90degree 1:on90degree) ==>"; cin >> a;
	//if (a == 0){
	//	demoMode = OFF_90_DEGREE;
	//}
	//else if (a == 1){
	//	demoMode == ON_90_DEGREE;
	//}
	//cout << "-------------------------------------" << endl;
	

}
//----------------------------------------------------------------------
int main ( int argc, char ** argv )
{
	glutInit ( &argc, argv );

	int window_X = floor_scale * 1.5;
	int window_Y = floor_scale;

	float aspect = window_X / (float)window_Y;

	initWindow(window_X * 1.5  , window_Y);
	initGlut();
	initScene();
	initTable();
	//initTexture();
	
	cam.setAspectRatio(aspect);

	setFurniturePosition();
	Metropolis_Hastings();
	glutMainLoop ();
	mallocFree();

    exit ( 0 );
}

void recordTotext(Furniture& objects){
		ofstream ofs("record_to_maya.txt");
		if (!ofs){
			cerr << "�t�@�C���I�[�v�����s" << endl;
			exit(1);
		}
		ofs << "trans[armchair] = " << objects.trans[0] << "   zrot[0] = " << objects.zrot[0] << endl;
		ofs << "trans[sofa] = " << objects.trans[1] << "   zrot[1] = " << objects.zrot[1] << endl;
		ofs << "trans[coffee_table] = " << objects.trans[2] << "   zrot[2] = " << objects.zrot[2] << endl;
		ofs << "trans[armchair] = " << objects.trans[3] << "   zrot[3] = " << objects.zrot[3] << endl;
		ofs << "trans[living_table] = " << objects.trans[4] << "   zrot[4] = " << objects.zrot[4] << endl;
		ofs << "trans[living_chair] = " << objects.trans[5] << "   zrot[5] = " << objects.zrot[5] << endl;
		ofs << "trans[living_chair] = " << objects.trans[6] << "   zrot[6] = " << objects.zrot[6] << endl;
		ofs << "trans[tv] = " << objects.trans[7] << "   zrot[7] = " << objects.zrot[7] << endl;
}

void recordmincost(clock_t start,clock_t end)
{
		ofstream ofs("time_min.txt");
		if (!ofs){
			cerr << "�t�@�C���I�[�v�����s" << endl;
			exit(1);
		}
		ofs << "loopCount = " << loopCount << endl;
		ofs << "time = " << (float)(end - start) / CLOCKS_PER_SEC << endl;
		ofs << "mincost = " << mincost << endl;
		ofs << "trans[armchair] = " << objects.trans[0] << "   zrot[0] = " << objects.zrot[0] << endl;
		ofs << "trans[sofa] = " << objects.trans[1] << "   zrot[1] = " << objects.zrot[1] << endl;
		ofs << "trans[coffee_table] = " << objects.trans[2] << "   zrot[2] = " << objects.zrot[2] << endl;
		ofs << "trans[armchair] = " << objects.trans[3] << "   zrot[3] = " << objects.zrot[3] << endl;
		ofs << "trans[living_table] = " << objects.trans[4] << "   zrot[4] = " << objects.zrot[4] << endl;
		ofs << "trans[living_chair] = " << objects.trans[5] << "   zrot[5] = " << objects.zrot[5] << endl;
		ofs << "trans[living_chair] = " << objects.trans[6] << "   zrot[6] = " << objects.zrot[6] << endl;
		ofs << "trans[tv] = " << objects.trans[7] << "   zrot[7] = " << objects.zrot[7] << endl;
}

void drawFurniture(Furniture& minobjects)
{
	//----authoring-------
	authoring_befor(minobjects);
	//-------------�ړ����܂߂�shade------------------
	calcAllLightIntensity3(ApointLight, AparallelLight, AspotLight, minobjects);
	//------------------------------------------------

	//minobjects.trans[0].x = -345.898; minobjects.trans[0].y = 20.3377; minobjects.zrot[0] = -90;
	//minobjects.trans[1].x = -184.034; minobjects.trans[1].y = -128.705; minobjects.zrot[1] = 0.400989;
	//minobjects.trans[2].x = -184.034; minobjects.trans[2].y = 20.3377; minobjects.zrot[2] = 0.400989;
	//minobjects.trans[3].x = -22.1705; minobjects.trans[3].y = 20.3377; minobjects.zrot[3] = -270;
	//minobjects.trans[4].x = 336.994; minobjects.trans[4].y = 23.09; minobjects.zrot[4] = 356.37;
	//minobjects.trans[5].x = 336.994; minobjects.trans[5].y = 234.73; minobjects.zrot[5] = 537.057;
	//minobjects.trans[6].x = 336.994; minobjects.trans[6].y = -188.55; minobjects.zrot[6] = -353.113;



	for (int i = 0; i < objectNum; i++){
		glPushMatrix();
		Tvector3<float> cen1 = minobjects.obj[i].getCenter();
		glTranslatef(minobjects.trans[i].x, minobjects.trans[i].y, minobjects.trans[i].z);
		glTranslatef(cen1.x, cen1.y, cen1.z);
		glRotatef(minobjects.zrot[i], 0.0, 0.0, 1.0);
		glTranslatef(-cen1.x, -cen1.y, -cen1.z);
		minobjects.obj[i].draw();
		glPopMatrix();
	}

	////-----�v�f���e�L�X�g�ɕۑ��imaya�����_�����O�̂��߁j--
	//recordTotext(minobjects);
	////-----------------------------------------------------


	//------------�e�X�g�p----------------------
	//calcLightDistribution2(minobjects, ApointLight);
	//------------------------------------------

	//floor
	glPushMatrix();
	glScalef(1.4, 1.4, 1.0);
	Floor.draw();
	glPopMatrix();

	//wall
	glPushMatrix();
	glScalef(1.4, 1.4, 1.5);
	Wall.draw();
	glPopMatrix();
}

void displayDrawDisk()
{
	//�~��\�����Ă܂�
	if (imode == HOPE_MODE){
		if (choiceFname == "sofa"){
			drawDisk(pick_point, radius - 5, radius, 1.5, 0.7);
		} 
		else{
			drawDisk(pick_point, radius - 5, radius);
		}
	}
	
	if (hope_flag == 1 && imode == TABLE_DEMO){
		drawDisk(p, 145, 150);
	}
}