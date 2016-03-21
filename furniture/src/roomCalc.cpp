#include "roomCalc.h"
#include "constants.h"
#include "myLight.h"
#include "calcAverageIlluminance.h"

void testIlluminance(vector<PointLight>& light);
const unsigned int channnelNum = 3; // RGB�Ȃ�3, RGBA�Ȃ�4
void* dataBuffer = (GLubyte*)malloc(WINDOW_SIZE_X * WINDOW_SIZE_Y * channnelNum);//free kaku korewo guro-barunisite free ha puroguramunosaigo

//GL�̏o�͂�CV�ɕς���
void saveImage(const unsigned int imageWidth, const unsigned int imageHeight, cv::Mat& gl_outImage)
{
	// �ǂݎ��OpneGL�̃o�b�t�@���w�� GL_FRONT:�t�����g�o�b�t�@�@GL_BACK:�o�b�N�o�b�t�@
	//glReadBuffer(GL_BACK);
	glReadBuffer(GL_FRONT);

	// OpenGL�ŉ�ʂɕ`�悳��Ă�����e���o�b�t�@�Ɋi�[
	glReadPixels(
		0,                 //�ǂݎ��̈�̍�������x���W
		0,                 //�ǂݎ��̈�̍�������y���W //0 or getCurrentWidth() - 1
		imageWidth,             //�ǂݎ��̈�̕�
		imageHeight,            //�ǂݎ��̈�̍���
		GL_BGR, //it means GL_BGR,           //�擾�������F���̌`��
		GL_UNSIGNED_BYTE,  //�ǂݎ�����f�[�^��ۑ�����z��̌^
		dataBuffer      //�r�b�g�}�b�v�̃s�N�Z���f�[�^�i���ۂɂ̓o�C�g�z��j�ւ̃|�C���^
		);

	GLubyte* p = static_cast<GLubyte*>(dataBuffer);
	//std::string fname = "outputImage.jpg";
	//IplImage* outImage = cvCreateImage(cvSize(imageWidth, imageHeight), IPL_DEPTH_8U, 3);

	for (unsigned int j = 0; j < imageHeight; ++j) //kokode kaunnto surebaiiyo
	{
		for (unsigned int i = 0; i < imageWidth; ++i)
		{
			gl_outImage.data[(imageHeight - j - 1)*gl_outImage.step + i * 3 + 0] = *p;
			gl_outImage.data[(imageHeight - j - 1)*gl_outImage.step + i * 3 + 1] = *(p + 1);
			gl_outImage.data[(imageHeight - j - 1)*gl_outImage.step + i * 3 + 2] = *(p + 2);
			p += 3;
		}
	}
	//cvSaveImage(fname.c_str(), outImage);
}

void mallocFree(){
	free(dataBuffer);
}

//clearance�͂܂������Ƃł��Ă܂���
float calcClearance(int window_size_x, int window_size_y)
{
	int mcv = 0;
	int count2 = 0;

	//glReadBuffer(GL_BACK);
	glReadBuffer(GL_FRONT);

	// OpenGL�ŉ�ʂɕ`�悳��Ă�����e���o�b�t�@�Ɋi�[
	glReadPixels(
		0,                 //�ǂݎ��̈�̍�������x���W
		0,                 //�ǂݎ��̈�̍�������y���W //0 or getCurrentWidth() - 1
		window_size_x,             //�ǂݎ��̈�̕�
		window_size_y,            //�ǂݎ��̈�̍���
		GL_BGR, //it means GL_BGR,           //�擾�������F���̌`��
		GL_UNSIGNED_BYTE,  //�ǂݎ�����f�[�^��ۑ�����z��̌^
		dataBuffer      //�r�b�g�}�b�v�̃s�N�Z���f�[�^�i���ۂɂ̓o�C�g�z��j�ւ̃|�C���^
		);

	GLubyte* p = static_cast<GLubyte*>(dataBuffer);

	for (unsigned int j = 0; j < window_size_x; ++j)
	{
		for (unsigned int i = 0; i < window_size_y; ++i)
		{
			if (*(p + 2) >= 255)
			{
				mcv++;
			}
			/*else if (*(p + 1) == 64)
			{
				count2++;
			}*/
			p += 3;
		}
	}
	//std::cout << "mcv(L) = " << mcv << "/* ������m�� */" << endl;
	//cout << "Cfree = " << count2 << "/* ������X�y�[�X */" << endl;

	return mcv / 50.0;
}
float calcCirculation(void)
{
	
	return 0;
}

float t(float d, float m, float M, int a = 2)
{
	float result = 1;
	if (d < m){
		for (int i = 0; i < a; i++){
			result *= (d / m);
		}
	}
	else if (d >= m && d <= M){
		result = 1.0;
	}
	else if (d > M){
		for (int i = 0; i < a; i++){
			result *= (M / d);
		}
	}
	return result;
}

float calcPairRelation(Furniture& objects,float& mpd,float& mpa)
{
	float p;
	float distance, mind, maxd, cosfg;
	mpd = 0,mpa = 0;

	for (int i = 0; i < objectNum; i++){
		for (int j = i + 1; j < objectNum; j++){
			Tvector3<float> d = (objects.trans[i]) - (objects.trans[j]);
			distance = d.length();

			Tvector3<float> f(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0),
				g(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);

			if ((objects.objname[i] == "armchair") && (objects.objname[j] == "coffee_table")){
				p = 1.0;
				mind = 130.0;
				maxd = 180.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if ((objects.objname[i] == "armchair") && (objects.objname[j] == "sofa")){
				p = 1.0;
				mind = 200.0;
				maxd = 240.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if ((objects.objname[i] == "sofa") && (objects.objname[j] == "armchair")){
				p = 1.0;
				mind = 200.0;
				maxd = 240.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if ((objects.objname[i] == "sofa") && (objects.objname[j] == "coffee_table")){
				p = 1.0;
				mind = 140.0;
				maxd = 210.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if ((objects.objname[i] == "armchair" && (objects.objname[j] == "armchair"))){
				p = 1.0;
				mind = 200.0;
				maxd = 350.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if (objects.objname[i] == "coffee_table" && objects.objname[j] == "armchair"){
				p = 1.0;
				mind = 130.0;
				maxd = 180.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if (objects.objname[i] == "armchair" && objects.objname[j] == "tv"){
				p = 1.0;
				mind = 200.0;
				maxd = 240.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if (objects.objname[i] == "sofa" && objects.objname[j] == "tv"){
				p = 1.0;
				mind = 280.0;
				maxd = 320.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			else if (objects.objname[i] == "coffee_table" && objects.objname[j] == "tv"){
				p = 1.0;
				mind = 140.0;
				maxd = 210.0;
				cosfg = dot(f, g);
				mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			}
			/*else if (objects.objname[i] == "coffee_table" && objects.objname[j] == "living_table"){
				p = 1.0;
				mind = 350;
				maxd = 400;
				cosfg = dot(f, g);
			}
			else if (objects.objname[i] == "sofa" && objects.objname[j] == "living_table"){
				p = 1.0;
				mind = 320;
				maxd = 420;
				cosfg = dot(f, g);
			}*/

			else if (objects.objname[i] == "living_table" && objects.objname[j] == "living_chair"){
				p = 1.0;
				mind = 80.0;
				maxd = 290.0;
				cosfg = dot(f, g);
				//mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
				mpa -= 2 * cosfg*cosfg - 1;
			}
			else if (objects.objname[i] == "living_table" && objects.objname[j] == "living_chair2"){
				p = 1.0;
				mind = 100.0;
				maxd = 340.0;
				cosfg = dot(f, g);
				mpa -= 2 * cosfg*cosfg - 1;
			}
			else{
				p = 0.0;
				mind = INT_MIN;
				maxd = INT_MAX;
			}
			mpd -= p*t(distance, mind, maxd, 2);
			//mpa -= 8 * cosfg*cosfg*cosfg*cosfg - 8 * cosfg*cosfg + 1; //�Ƌ90�x�܂���180�x�̂Ƃ��ő�ɂȂ�
			
		}
	}
	return 0;
}

float calcConversation(Furniture& objects,float& mcd,float& mca)
{
	bool flag = false;
	float q, cosfg, cosgf, distance, maxd, mind;
	mcd = 0.0, mca = 0.0;
	for (int i = 0; i < objectNum; i++){
		for (int j = i + 1; j < objectNum; j++){
			Tvector3<float> d = (objects.trans[i]) - (objects.trans[j]);
			distance = d.length();
			d.x /= distance;
			d.y /= distance;
			d.z = 0.0;
			if (objects.objname[i] == "armchair" && objects.objname[j] == "sofa" && flag == false){
				q = 1.0; mind = 200.0; maxd = 240.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "armchair" && objects.objname[j] == "armchair" && flag == false){
				q = 1.0; mind = 250.0; maxd = 350.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "sofa" && objects.objname[j] == "armchair" && flag == false){
				q = 1.0; mind = 200.0; maxd = 240.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "armchair" && objects.objname[j] == "tv" && flag == false){
				q = 1.0;
				mind = 200.0; maxd = 240.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "sofa" && objects.objname[j] == "tv" && flag == false){
				q = 1.0; mind = 280.0; maxd = 320.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "living_chair" && objects.objname[j] == "living_chair" && flag == false){
				q = 1.0; mind = 350.0; maxd = 400.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "living_chair" && objects.objname[j] == "living_chair2" && flag == false){
				q = 1.0; mind = 130.0; maxd = 180.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else if (objects.objname[i] == "living_chair2" && objects.objname[j] == "living_chair2" && flag == false){
				q = 1.0; mind = 350.0; maxd = 400.0;
				Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), n2(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
				cosfg = dot(n1, -d); cosgf = dot(n2, d);
				flag = true;
			}
			else{
				q = 0.0; mind = INT_MIN; maxd = INT_MAX;
				cosfg = 0.0; cosgf = 0.0;
				flag = true;
			}
			if (flag == true){
				mcd -= q*t(distance, mind, maxd, 2);
				mca -= q*(cosfg + 1.0)*(cosgf + 1.0);
				flag = false;
			}
		}
	}
	return 0;
}

//����H�H[]����Y��Ă����ǂ���ʖڂ���Ȃ����H�H�H
float calcVisualBalance(Furniture& objects)
{
	Tvector3<float> centroid(0.0, 0.0, 0.0), roomCenter(0.0, 0.0, 0.0); //roomCenter���X���邱�Ƃ��ł���
	float sumArea = 0.0, oneArea, mvb;

	//����H�H[]����Y��Ă����ǂ���ʖڂ���Ȃ����H�H�H
	for (int i = 0; i < objectNum; i++){
		//oneArea = (objects.obj.getMaximumPoint().x - objects.obj.getMinimumPoint().x)
		//	*(objects.obj.getMaximumPoint().y - objects.obj.getMinimumPoint().y)
		//	*(objects.obj.getMaximumPoint().z - objects.obj.getMinimumPoint().z);
		oneArea = (objects.obj[i].getMaximumPoint().x - objects.obj[i].getMinimumPoint().x)
			*(objects.obj[i].getMaximumPoint().y - objects.obj[i].getMinimumPoint().y);
			//*(objects.obj[i].getMaximumPoint().z - objects.obj[i].getMinimumPoint().z);
		centroid += oneArea * (objects.trans[i]);
		centroid.z = 0.0;
		sumArea += oneArea;
	}
	centroid /= sumArea;

	mvb = (centroid - roomCenter).length();
	return mvb/100.0;
	
}

//����H�H[]����Y��Ă����ǂ���ʖڂ���Ȃ����H�H�H
void calcAlignment(Furniture& objects,float& mfa, float& mwa)
{
	mfa = 0.0, mwa = 0.0;
	float cosf, cosg, sinf, sing, temp, cosw, sinw, tempw, sumArea = 0.0, oneArea, sumArea2 = 0.0, oneArea2;
	float cosw2, sinw2;
	Tvector3<float> normalx(1.0, 0.0, 0.0), normaly(0.0, 1.0, 0.0), centroid, centroid2;

	for (int i = 0; i < objectNum; i++){
		for (int j = i+1; j < objectNum; j++){
			Tvector3<float> f(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0), 
				g(-sin(TO_RAD(objects.zrot[j])), cos(TO_RAD(objects.zrot[j])), 0.0);
			cosf = dot(f, normalx);
			sinf = corss(f, normalx).length();
			cosg = dot(g, normalx);
			sing = corss(g, normalx).length();
			temp = cosf*cosg + sinf*sing;
			mfa -= 8 * temp*temp*temp*temp - 8 * temp*temp + 1;
		}
	}

	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "coffee_table" || objects.objname[i] == "armchair" ||
			objects.objname[i] == "sofa" || objects.objname[i] == "tv"){
			oneArea = (objects.obj[i].getMaximumPoint().x - objects.obj[i].getMinimumPoint().x)
			*(objects.obj[i].getMaximumPoint().y - objects.obj[i].getMinimumPoint().y);
			centroid += oneArea * (objects.trans[i]);
			centroid.z = 0.0;
			sumArea += oneArea;
		}
		else if (objects.objname[i] == "living_table" || objects.objname[i] == "living_chair"){
			oneArea2 = (objects.obj[i].getMaximumPoint().x - objects.obj[i].getMinimumPoint().x)
			*(objects.obj[i].getMaximumPoint().y - objects.obj[i].getMinimumPoint().y);
			centroid2 += oneArea2 * (objects.trans[i]);
			centroid2.z = 0.0;
			sumArea2 += oneArea2;
		}
	}
	centroid /= sumArea;
	centroid /= sumArea2;

	////����H[]����Y��Ă����ǂ���ʖڂ���ˁH�H
	//for (int i = 0; i < objectNum; i++){
	//	//oneArea = (objects.obj.getMaximumPoint().x - objects.obj.getMinimumPoint().x)
	//	//	*(objects.obj.getMaximumPoint().y - objects.obj.getMinimumPoint().y)
	//	//	*(objects.obj.getMaximumPoint().z - objects.obj.getMinimumPoint().z);
	//	oneArea = (objects.obj[i].getMaximumPoint().x - objects.obj[i].getMinimumPoint().x)
	//		*(objects.obj[i].getMaximumPoint().y - objects.obj[i].getMinimumPoint().y);
	//		//*(objects.obj[i].getMaximumPoint().z - objects.obj[i].getMinimumPoint().z);
	//	centroid += oneArea * (objects.trans[i]);
	//	centroid.z = 0.0;
	//	sumArea += oneArea;
	//}
	//centroid /= sumArea;

	

	if (centroid.x > 0 && centroid.y < centroid.x && centroid.y > -centroid.x){
		cosw = dot(normalx, normaly);
		sinw = corss(normalx, normaly).length();
		//cosw = 0.0;
		//sinw = 1.0;
	}
	else if (centroid.y < 0 && centroid.y < centroid.x && centroid.y < -centroid.x){
		cosw = dot(normalx, -normalx);
		sinw = 0.0;
		//cosw = -1.0;
		//sinw = 0.0;
	}
	else if (centroid.x < 0 && centroid.y > centroid.x && centroid.y < -centroid.x){
		cosw = dot(normalx, -normaly);
		sinw = corss(normalx, -normaly).length();
		//cosw = 0.0;
		//sinw = -1.0;
	}
	else if (centroid.y > 0 && centroid.y > centroid.x && centroid.y > -centroid.x){
		cosw = dot(normalx, normalx);
		sinw = 0.0;
		//cosw = 1.0;
		//sinw = 0.0;
	}


	if (centroid2.x > 0 && centroid2.y < centroid2.x && centroid2.y > -centroid2.x){
		cosw2 = dot(normalx, normaly);
		sinw2 = corss(normalx, normaly).length();
		//cosw = 0.0;
		//sinw = 1.0;
	}
	else if (centroid2.y < 0 && centroid2.y < centroid2.x && centroid2.y < -centroid2.x){
		cosw2 = dot(normalx, -normalx);
		sinw2 = 0.0;
		//cosw = -1.0;
		//sinw = 0.0;
	}
	else if (centroid2.x < 0 && centroid2.y > centroid2.x && centroid2.y < -centroid2.x){
		cosw2 = dot(normalx, -normaly);
		sinw2 = corss(normalx, -normaly).length();
		//cosw = 0.0;
		//sinw = -1.0;
	}
	else if (centroid2.y > 0 && centroid2.y > centroid2.x && centroid2.y > -centroid2.x){
		cosw2 = dot(normalx, normalx);
		sinw2 = 0.0;
		//cosw = 1.0;
		//sinw = 0.0;
	}

	for (int i = 0; i < objectNum; i++){
		Tvector3<float> f(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0);
		cosf = dot(f, normalx);
		sinf = corss(f, normalx).length();
		if (objects.objname[i] == "coffee_table" || objects.objname[i] == "armchair" ||
			objects.objname[i] == "sofa" || objects.objname[i] == "tv"){
			tempw = cosf*cosw + sinf*sinw;
		}
		else if (objects.objname[i] == "living_table" || objects.objname[i] == "living_chair"){
			tempw = cosf*cosw2 + sinf*sinw2;
		}
		mwa -= 8 * tempw*tempw*tempw*tempw - 8 * tempw*tempw + 1;
	}
	
}

float calcEmphasis(Furniture& objects)
{
	//291.167 = floorX/2.0
	Tvector3<float> ps(0.0, 291.167, 0.0);
	Tvector3<float> pd;//�Ⴆ�΂̐ݒ�
	string a;
	float mef = 0.0;
	for (int i = 0; i < objectNum; i++){
		pd = ps - objects.trans[i];
		pd /= pd.length();
		Tvector3<float> n1(-sin(TO_RAD(objects.zrot[i])), cos(TO_RAD(objects.zrot[i])), 0.0);
		mef -= dot(pd, n1);
	}
	return mef;
}

float calcNormalDistribution(float mu, float sigma, float x)
{
	return ((1 / sqrtf(2 * M_PI * sigma * sigma))*exp2f(-((x - mu)*(x - mu)) / (2 * sigma * sigma))) / (1 / sqrtf(2 * M_PI * sigma * sigma));
}

float calcHopeDistribution(Furniture& objects)
{
	float mhd = 0;
	//table�̐��K���z�ɂ������]���z
	mhd -= calcNormalDistribution(0.0, 33.0, objects.trans[2].x)*calcNormalDistribution(38.25, 33.0, objects.trans[2].y);

	//sofa�̐��K���z�ɂ������]���z
	mhd -= calcNormalDistribution(0.0, 70.0, objects.trans[1].x)*calcNormalDistribution(-112.75, 33, objects.trans[1].y);

	//armchair�̐��K���z�ɂ������]���z

	return mhd;
}

float calcHopeDistribution3(Furniture& objects)
{
	float mhd = 0;

	for (int i = 0; i < objectNum; i++){
		if (objects.hopeRadius[i] > 0){
			mhd -= calcNormalDistribution(objects.hopeCenter[i].x, /*(1 / 3)**/ objects.hopeRadius[i] * objects.hopeScalex[i], objects.trans[i].x)
			*calcNormalDistribution(objects.hopeCenter[i].y, /*(1 / 3)**/ objects.hopeRadius[i] * objects.hopeScaley[i], objects.trans[i].y);
		}
	}
	return mhd;
}

float Uniform(void){
	return ((float)rand() + 1.0) / ((float)RAND_MAX + 2.0);
}

float rand_normal(float mu, float sigma){
	float z = sqrt(-2.0*log(Uniform())) * sin(2.0*M_PI*Uniform());
	return mu + sigma*z;
}

//tv�����Ă�΁[�����
void authoring(Furniture& objects){
	int sofa, coffee_table, armchair[2] = { -1, -1}, living_table, living_chair[4] = { -1, -1, -1, -1},tv;
	
	//�Ƌ��ID�t��
	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "sofa"){
			sofa = i;
		}
		else if (objects.objname[i] == "coffee_table"){
			coffee_table = i;
		}
		else if (objects.objname[i] == "armchair" && armchair[0] < 0){
			armchair[0] = i;
		}
		else if (objects.objname[i] == "armchair" && armchair[0] >= 0){
			armchair[1] = i;
		}
		else if (objects.objname[i] == "living_table"){
			living_table = i;
		}
		else if (objects.objname[i] == "living_chair" && living_chair[0] < 0){
			living_chair[0] = i;
		}
		else if (objects.objname[i] == "living_chair" && living_chair[0] >= 0){
			living_chair[1] = i;
		}
		else if (objects.objname[i] == "tv"){
			tv = i;
		}
	}

	////living_table��living_chair�̔�����
	//Tvector3<float> n(-sin(TO_RAD(objects.zrot[living_table])), cos(TO_RAD(objects.zrot[living_table])), 0.0);
	//float co = dot(n, Tvector3<float>(0.0, 1.0, 0.0));
	//if (2 * co*co - 1 > 0){
	//	objects.trans[living_chair[0]].x = objects.trans[living_table].x;
	//	objects.trans[living_chair[1]].x = objects.trans[living_table].x;
	//	objects.trans[living_table].y = (objects.trans[living_chair[0]].y + objects.trans[living_chair[1]].y) / 2.0;
	//}
	//else if (2 * co*co - 1 < 0){
	//	objects.trans[living_chair[0]].y = objects.trans[living_table].y;
	//	objects.trans[living_chair[1]].y = objects.trans[living_table].y;
	//	objects.trans[living_table].x = (objects.trans[living_chair[0]].x + objects.trans[living_chair[1]].x) / 2.0;
	//}

	//�\�t�@�ƃR�[�q�[�e�[�u���̌��������낦��
	objects.zrot[coffee_table] = objects.zrot[sofa];

	//tv�̒���
	Tvector3<float> temp(objects.trans[sofa].x - objects.trans[coffee_table].x, objects.trans[sofa].y - objects.trans[coffee_table].y, 0.0);
	objects.trans[tv].x = objects.trans[coffee_table].x - temp.x;
	objects.trans[tv].y = objects.trans[coffee_table].y - temp.y;

	////sofa����̔������������Ⴄ��

	//if ((int)objects.zrot[sofa] % 90 != 0){
	//	float restsofa = (int)objects.zrot[sofa] % 90;
	//	cout << "amari = " << restsofa << endl;
	//	objects.zrot[sofa] = 180;
	//}
	//objects.zrot[coffee_table] = objects.zrot[sofa];



	////�\�t�@�ƃR�[�q�[�e�[�u���̌��������낦��(������Ɣ����c)
	//Tvector3<float> n1(-sin(TO_RAD(objects.zrot[sofa])), cos(TO_RAD(objects.zrot[sofa])), 0.0);
	//float co1 = dot(n1, Tvector3<float>(0.0, 1.0, 0.0));
	//float sin1 = corss(n1, Tvector3<float>(0.0, 1.0, 0.0)).length();
	//
	//Tvector3<float> temp = (objects.trans[armchair[0]] + objects.trans[armchair[1]]) / 2.0;
	////sofa��������܂��͉�����
	//if (2 * co1*co1 - 1 > 0){
	//	objects.trans[armchair[0]].y = temp.y;
	//	objects.trans[armchair[1]].y = temp.y;
	//	objects.trans[coffee_table].x = temp.x;
	//	objects.trans[coffee_table].y = temp.y;
	//	objects.trans[sofa].x = temp.x;
	//}
	////sofa���E�����܂��͍�����
	//else if (2 * co1*co1 - 1 < 0){
	//	objects.trans[armchair[0]].x = temp.x;
	//	objects.trans[armchair[1]].x = temp.x;
	//	objects.trans[coffee_table].y = temp.y;
	//	objects.trans[coffee_table].x = temp.x;
	//	objects.trans[sofa].y = temp.y;
	//}
}

//tv�����ĂȂ��o�[�W����
void authoring_befor(Furniture& objects){
	int sofa, coffee_table, armchair[2] = { -1, -1 }, living_table, living_chair[4] = { -1, -1, -1, -1 };

	//�Ƌ��ID�t��
	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "sofa"){
			sofa = i;
		}
		else if (objects.objname[i] == "coffee_table"){
			coffee_table = i;
		}
		else if (objects.objname[i] == "armchair" && armchair[0] < 0){
			armchair[0] = i;
		}
		else if (objects.objname[i] == "armchair" && armchair[0] >= 0){
			armchair[1] = i;
		}
		else if (objects.objname[i] == "living_table"){
			living_table = i;
		}
		else if (objects.objname[i] == "living_chair" && living_chair[0] < 0){
			living_chair[0] = i;
		}
		else if (objects.objname[i] == "living_chair" && living_chair[0] >= 0){
			living_chair[1] = i;
		}
	}

	//living_table��living_chair�̔�����
	Tvector3<float> n(-sin(TO_RAD(objects.zrot[living_table])), cos(TO_RAD(objects.zrot[living_table])), 0.0);
	float co = dot(n, Tvector3<float>(0.0, 1.0, 0.0));
	if (2 * co*co - 1 > 0){
		objects.trans[living_chair[0]].x = objects.trans[living_table].x;
		objects.trans[living_chair[1]].x = objects.trans[living_table].x;
		objects.trans[living_table].y = (objects.trans[living_chair[0]].y + objects.trans[living_chair[1]].y) / 2.0;
	}
	else if (2 * co*co - 1 < 0){
		objects.trans[living_chair[0]].y = objects.trans[living_table].y;
		objects.trans[living_chair[1]].y = objects.trans[living_table].y;
		objects.trans[living_table].x = (objects.trans[living_chair[0]].x + objects.trans[living_chair[1]].x) / 2.0;
	}

	//�\�t�@�ƃR�[�q�[�e�[�u���̌��������낦��
	objects.zrot[coffee_table] = objects.zrot[sofa];

	//�\�t�@�ƃR�[�q�[�e�[�u���̌��������낦��(������Ɣ����c)
	Tvector3<float> n1(-sin(TO_RAD(objects.zrot[sofa])), cos(TO_RAD(objects.zrot[sofa])), 0.0);
	float co1 = dot(n1, Tvector3<float>(0.0, 1.0, 0.0));
	float sin1 = corss(n1, Tvector3<float>(0.0, 1.0, 0.0)).length();

	Tvector3<float> temp = (objects.trans[armchair[0]] + objects.trans[armchair[1]]) / 2.0;
	//sofa��������܂��͉�����
	if (2 * co1*co1 - 1 > 0){
		objects.trans[armchair[0]].y = temp.y;
		objects.trans[armchair[1]].y = temp.y;
		objects.trans[coffee_table].x = temp.x;
		objects.trans[coffee_table].y = temp.y;
		objects.trans[sofa].x = temp.x;
		
		//�A�[���`�F�A�p�x����
		if (objects.trans[armchair[0]].x > objects.trans[armchair[1]].x){
			objects.zrot[armchair[0]] = -270.0;
			objects.zrot[armchair[1]] = -90.0;
		}
		else if (objects.trans[armchair[0]].x < objects.trans[armchair[1]].x){
			objects.zrot[armchair[0]] = -90.0;
			objects.zrot[armchair[1]] = -270.0;
		}

		
	}
	//sofa���E�����܂��͍�����
	else if (2 * co1*co1 - 1 < 0){
		objects.trans[armchair[0]].x = temp.x;
		objects.trans[armchair[1]].x = temp.x;
		objects.trans[coffee_table].y = temp.y;
		objects.trans[coffee_table].x = temp.x;
		objects.trans[sofa].y = temp.y;

		//�A�[���`�F�A�p�x����
		if (objects.trans[armchair[0]].y > objects.trans[armchair[1]].y){
			objects.zrot[armchair[0]] = 180.0;
			objects.zrot[armchair[1]] = 360.0;
		}
		else if (objects.trans[armchair[0]].y < objects.trans[armchair[1]].y){
			objects.zrot[armchair[0]] = 360.0;
			objects.zrot[armchair[1]] = 180.0;
		}


	}

	////�A�[���`�F�A�̊p�x����
	//Tvector3<float> n0arm(-sin(TO_RAD(objects.zrot[armchair[0]])), cos(TO_RAD(objects.zrot[armchair[0]])), 0.0);
	//Tvector3<float> n1arm(-sin(TO_RAD(objects.zrot[armchair[0]])), cos(TO_RAD(objects.zrot[armchair[0]])), 0.0);
	//float co_arm = dot(n0arm, n1arm);
	//if (co_arm != 1 || co_arm != 1){
	//	objects.zrot[armchair[0]] = 0.0;
	//	objects.zrot[armchair[1]] = 180.0;
	//}

}

//��������Ȃ�==================================================================================
float calcGrayIntensity(vector<PointLight>& light, Furniture& objects, int i)
{
	Tcolor3<float> c;
	float gray;
	Tmaterial& mat = objects.obj[i].getMaterial();
	float tx = objects.trans[i].x;
	float ty = objects.trans[i].y;
	float hz = objects.obj[i].getMaximumPoint().z - objects.obj[i].getMinimumPoint().z;
	for (int j = 0; j < light.size(); j++){
		if (j == 0){
			c = light[j].shade(Tvector3<float>(tx, ty, hz), Tvector3<float>(0.0, 0.0, 1.0), mat.getDiffuse());
		}
		else if (i != 0){
			c += light[j].shade(Tvector3<float>(tx, ty, hz), Tvector3<float>(0.0, 0.0, 1.0), mat.getDiffuse());
		}
	}
	gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
	return gray;

	//float gray, x0, y0, mld = 0;
	//int quotientX, quotientY, vtx;		//quotient...��

	//x0 = floor[0].getVertex(0).getPosition().x;
	//y0 = floor[0].getVertex(0).getPosition().y;

	//for (int i = 0; i < objectNum; i++){
	//	quotientX = -(objects.trans[i].x - x0) / 28.0;
	//	quotientY = (objects.trans[i].y - y0) / 28.0;
	//	vtx = (50 + 1) * quotientY + quotientX;

	//	gray = 0.3*floor[0].getVertex(vtx).getColor().r + 0.59*floor[0].getVertex(vtx).getColor().g + 0.11*floor[0].getVertex(vtx).getColor().b;
	//	mld -= gray;
	//}

	//return 0;
	//

}
//������p�������z�̃R�X�g�v�Z �O���[�X�P�[���ɕϊ����Ă܂���
float calcLightDistribution(Furniture& objects, vector<PointLight>& pointLight, vector<SpotLight>& spotLight)
{
	float mld = 0, lx, m, M, r;
	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "coffee_table"){
			r = 1.0;
			lx = calcGrayIntensity(pointLight, objects, i);
			//cout << "lx(coffee_table) = " << lx << endl;
			m = 0.1;
			M = 0.6;
		}
		else if (objects.objname[i] == "living_table"){
			r = 0.0;
			lx = calcGrayIntensity(pointLight, objects, i);
			//cout << "lx(living_table) = " << lx << endl;
			m = 1.0;
			M = 3.0;
		}
		else{
			r = 0.0;
			lx = 0.0;
			m = 1.0;
			M = 1.0;
		}
		mld -= r*t(lx, m, M, 3);
	}

	return mld;
}
//�e�X�g�p
float calcLightDistribution2(Furniture& objects, vector<PointLight>& light)
{
	//float gray, x0, y0, mld = 0;
	//int quotientX, quotientY, vtx;		//quotient...��

	//x0 = floor[0].getVertex(0).getPosition().x;
	//y0 = floor[0].getVertex(0).getPosition().y;

	//for (int i = 0; i < objectNum; i++){
	//	quotientX = -(objects.trans[i].x - x0) / 28.0;
	//	quotientY = (objects.trans[i].y - y0) / 28.0;
	//	vtx = (50 + 1) * quotientY + quotientX;

	//	gray = 0.3*floor[0].getVertex(vtx).getColor().r + 0.59*floor[0].getVertex(vtx).getColor().g + 0.11*floor[0].getVertex(vtx).getColor().b;
	//	mld -= gray;
	//}

	//return 0;
	//


	float mld = 0, lx, m, M, r;
	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "coffee_table"){
			r = 1.0;
			lx = calcGrayIntensity(light, objects, i);
			std::cout << "lx(coffee_table) = " << lx << endl;
			m = 0.1;
			M = 0.7;
		}
		else if (objects.objname[i] == "living_table"){
			r = 0.0;
			lx = calcGrayIntensity(light, objects, i);
			std::cout << "lx(living_table) = " << lx << endl;
			m = 0.7;
			M = 1.5;
		}
		else{
			r = 0.0;
			lx = 0.0;
			m = 1.0;
			M = 1.0;
		}
		//mld -= r*t(lx, m, M, 2);
	}

	return 0;
}
//==============================================================================================

//�����̍�����2.4m(���W��240)�@�܂�A100�Ŋ���B
float calcLightIlluminance(Furniture& objects,vector<PointLight>& light, int i)
{
	float lx = 0;
	float tx = objects.trans[i].x / 100.0;
	float ty = objects.trans[i].y / 100.0;
	float hz = ((objects.obj[i].getMaximumPoint().z - objects.obj[i].getMinimumPoint().z) + objects.trans[i].z) / 100.0;
	Tvector3<float> trans(tx, ty, hz);
	//cout << "trans ( " << objects.objname[i] << " ) = " << trans << endl;
	for (int j = 0; j < light.size(); j++){
		//position_light /= 100.0;
		//std::cout << "trans ( light["<< j <<"] ) = " << light[j].getPosition() << endl;
		Tvector3<float> dif = (light[j].getPosition() / 100.0 - trans);
		//std::cout << "dif = " << dif << endl;
		float difference = dif.lengthSquared();
		//std::cout << "difference = " << difference << endl;
		//std::cout << "cd = " << light[j].getLuminosity() << endl;
		//std::cout << "lx ("<<objects.objname[i]<<") = " << light[j].getLuminosity() / difference << endl;;
		lx += light[j].getLuminosity() / difference;
	}
	//std:: cout << "============================" << endl;
	//std:: cout << "total lx = " << lx << endl;
	return lx;
}

//��_�̏Ɠx��p�����]���֐�
float calcIlluminanceDistribution(Furniture& objects, vector<PointLight>& light)
{
	float mld = 0, lx, mlx, Mlx, r;
	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "coffee_table"){
			r = 1.0;
			//mlx = 100.0;
			//Mlx = 200.0;
			mlx = 150;
			Mlx = 220;
			lx = calcLightIlluminance(objects, light, i);
			//cout << "lx(coffee_table) = " << lx << endl;
		}
		else if (objects.objname[i] == "living_table"){
			r = 1.0;
			//mlx = 200.0;
			//Mlx = 300.0;
			mlx = 220;
			Mlx = 500;
			lx = calcLightIlluminance(objects, light, i);
			//cout << "lx(living_table) = " << lx << endl;
		}
		else{
			r = 0.0;
			mlx = 0.0;
			Mlx = 0.0;
			lx = 0.0;
		}
		mld -= r*t(lx, mlx, Mlx, 3);
	}
	//cout << "mld = " << mld << endl;
	//testIlluminance(light);

	return mld;

}

//��_�̏Ɠx���m�F����
void testIlluminance(vector<PointLight>& light,Furniture& objects)
{
	Tvector3<float> point1(light[0].getPosition().x / 100.0, 0.0, 0.0), point2(-light[0].getPosition().x / 100.0, 0.0, 0.0);
	float lx1 = 0, lx2 = 0;

	point1.z = (objects.obj[2].getMaximumPoint().z - objects.obj[2].getMinimumPoint().z) / 100.0;//coffee_table
	point2.z = (objects.obj[4].getMaximumPoint().z - objects.obj[4].getMinimumPoint().z) / 100.0;//living_table

	for (int i = 0; i < light.size(); i++){
		Tvector3<float> diff1 = light[i].getPosition() / 100.0 - point1;
		Tvector3<float> diff2 = light[i].getPosition() / 100.0 - point2;

		float difference1 = diff1.lengthSquared();
		float difference2 = diff2.lengthSquared();

		lx1 += light[i].getLuminosity() / difference1;
		lx2 += light[i].getLuminosity() / difference2;
	}
	
	cout << "========================================" << endl;
	cout << "lx(�d�_�Ɩ��i���j,coffee_table) = " << lx1 << endl;
	cout << "lx(�d�_�Ɩ��i��j,living_table) = " << lx2 << endl;
	cout << "----------------------------------------" << endl;

	lx1 = 0.0;
	lx2 = 0.0;

	point1.z = (objects.obj[4].getMaximumPoint().z - objects.obj[4].getMinimumPoint().z) / 100.0; //living_table
	point2.z = (objects.obj[2].getMaximumPoint().z - objects.obj[2].getMinimumPoint().z) / 100.0; //coffee_table

	for (int i = 0; i < light.size(); i++){
		Tvector3<float> diff1 = light[i].getPosition() / 100.0 - point1;
		Tvector3<float> diff2 = light[i].getPosition() / 100.0 - point2;

		float difference1 = diff1.lengthSquared();
		float difference2 = diff2.lengthSquared();

		lx1 += light[i].getLuminosity() / difference1;
		lx2 += light[i].getLuminosity() / difference2;
	}

	cout << "lx(�d�_�Ɩ��i���j,living_table) = " << lx1 << endl;
	cout << "lx(�d�_�Ɩ��i��j,coffee_table) = " << lx2 << endl;
	cout << "========================================" << endl;
}

//���ϏƓx��p�����]���֐�
float calcAverageIlluminanceDistribution(vector<vector<float>>& livingFluxTable,vector<vector<float>>& coffeeFluxTable, float living_height, float coffee_height, int livingNum, int coffeeNum, Furniture& objects)
{
	float kbri = 190;
	float coffee_average = 0.035711*2;
	float dining_average = 0.025517*2;
	//float coffee_average = 0.101364;
	//float dining_average = 0.087199;
	float mld = 0, lx, mlx, Mlx, r;
	for (int i = 0; i < objectNum; i++){
		if (objects.objname[i] == "coffee_table"){
			r = 1.0; mlx = 150; Mlx = 250;
			lx = calcAverageIlluminance(coffeeFluxTable, coffee_height, coffeeNum, objects);

			////���ݔ��˂̍����l������
			lx += coffee_average * kbri ;

			//cout << "coffee_table = ��/�r = " << lx << endl;
		}
		else if (objects.objname[i] == "living_table"){
			r = 1.0; mlx = 250; Mlx = 430;
			lx = calcAverageIlluminance(livingFluxTable, living_height, livingNum, objects);
			
			////���ݔ��˂̍����l������
			lx += dining_average * kbri ;
			
			//cout << "living_table = ��/�r = " << lx << endl;
		}
		else{
			r = 0.0; mlx = 0.0; Mlx = 0.0;
			lx = 0.0;
		}
		mld -= r*t(lx, mlx, Mlx, 3);
	}
	return mld;

}

//���ϏƓx���m�F����֐�
void testAverageIlluminance(vector<vector<float>>& fluxTable, vector<PointLight>& pointLight, vector<SpotLight>& spotLight, Furniture& objects)
{

}