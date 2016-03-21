#define _MYLIGHT_CPP_

#include "myLight.h"
#include "constants.h"

//�_�����̃V�F�[�f�B���O
Tcolor3<float> PointLight::shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse)
{
	Tvector3<float> vec = position - p;
	//cm��m�ɕϊ�
	vec = vec / 100.0;
	float r2 = vec.lengthSquared();
	vec.normalize();
	float co = vec.dot(n);
	Tcolor3<float> c = intensity * diffuse * color * co / r2;
	return c;
}

////-----------------------------------------------------------------
//void TobjectManager::shadeObjects(TparallelLight& light) {
//	for (int io = 0; io<numObject; io++) {
//		Tcolor3<float> ref = object[io].getMaterial().getDiffuse();
//		for (int iv = 0; iv<object[io].getNumVertex(); iv++) {
//			Tvertex *v = object[io].getVertexAddress(iv);
//			Tvector3<float> n = v->getNormal();
//			float co = MAX(0.0, n.dot(light.getDirection()));
//			Tcolor3<float> color = co*ref*light.getColor()*light.getIntensity() + ref * light.getAmbient();
//			v->setColor(color);
//		}
//	}
//}
////-----------------------------------------------------------------

//���s�����̃V�F�[�f�B���O
Tcolor3<float> ParallelLight::shade(Tvector3<float> n, Tcolor3<float> diffuse)
{
	float co = n.dot(direction);
	Tcolor3<float> c = intensity * diffuse * color * co;
	return c;
}

//�X�|�b�g���C�g�̃V�F�[�f�B���O
Tcolor3<float> SpotLight::shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse)
{
	float costhN = 1.0;
	//Tvector3<float> light_axis(0.0, 0.0, -1.0);
	Tvector3<float> vec = position - p;
	//cm��m�ɕϊ�
	vec = vec / 100.0;
	float r2 = vec.lengthSquared();
	vec.normalize();
	//float costh = light_axis.dot(-vec);
	float costh = direction.dot(-vec);
	float cosalfa = vec.dot(n);
	Tcolor3<float> c = intensity * diffuse * color * cosalfa / r2;

	for (int i = 0; i < directivity; i++){
		costhN *= costh;
	}

	c *= costhN;

	return c;

}

//��񔽎˂�ǂ̃��b�V���̓_�����ōl����
void calcOnceRefraction(vector<Tvector3<float>>& wallpoint, vector<PointLight>& pointLight, vector<SpotLight>& spotLight,
	int pnum, int snum, int imesh/*���̃��b�V���̐�*/, int jmesh/*�c�̃��b�V���̐�*/, float reflectance, Tvector3<float> normal, Tcolor3<float>& color)
{
	Tvector3<float> center, wvec, hvec;
	float dx, dy, width, height;
	PointLight wallpointLight;
	
	wvec = wallpoint[1] - wallpoint[0];
	hvec = wallpoint[3] - wallpoint[0];
	if (normal.x != 0)/*���E�̕�*/{
		width = abs(wallpoint[0].y - wallpoint[1].y);
		height = abs(wallpoint[0].z - wallpoint[3].z);
		dx = width / (float)imesh;
		dy = height / (float)jmesh;
		cout << "���E�̕�" << endl;
	}
	/*���s���̕�*/
	else if (normal.y != 0){
		width = abs(wallpoint[0].x - wallpoint[1].x);
		height = abs(wallpoint[0].z - wallpoint[3].z);
		dx = width / (float)imesh;
		dy = height / (float)jmesh;
		cout << "���C��O�̕�" << endl;
	}
	/*�V��*/
	else if (normal.z != 0){
		width = abs(wallpoint[0].x - wallpoint[1].x);
		height = abs(wallpoint[0].y - wallpoint[3].y);
		dx = width / (float)imesh;
		dy = height / (float)jmesh;
		cout << "�V��" << endl;
	}

	// intensity:shade�̂��߂̌����̋��x�Cluminosity:�Ɠx�̂��߂̌��x
	float intensity = 0, luminosity = 0, r2length, co, cospot, spotintensity, spotluminosity;
	// �Ɩ��ƕǌ����̋����C��ŒP�ʃx�N�g���ɂ��܂�
	Tvector3<float> r;
	for (int i = 0; i < imesh; i++){
		for (int j = 0; j < jmesh; j++){
			center = wallpoint[0] + ((i + 0.5)*dx / width)*wvec + ((j + 0.5)*dy / height)*hvec;
			//cm��m�ɕϊ�
			dx = dx / 100.0; dy = dy / 100.0;
			//pointLight�̂Ƃ��̕ǂ̔���
			for (int p = 0; p < pnum; p++){
				r = pointLight[p].getPosition() - center; r = r / 100.0;	/*cm��m�ɕϊ�*/
				r2length = r.lengthSquared(); r = r.normalize();
				if (r2length < 1){
					r2length = 1.0;
				}
				co = r.dot(normal);
				intensity += pointLight[p].getIntensity()*co*reflectance*dx*dy / r2length;
				luminosity += pointLight[p].getLuminosity()*co*reflectance*dx*dy / r2length;
			}
			//spotLight�̂Ƃ��̕ǂ̔���
			for (int s = 0; s < snum; s++){
				r = spotLight[s].getPosition() - center; r = r / 100.0;		/*cm��m�ɕϊ�*/
				r2length = r.lengthSquared(); r = r.normalize();
				if (r2length < 1){
					r2length = 1.0;
				}
				co = r.dot(normal);
				cospot = spotLight[s].getDirection().dot(-r);
				spotintensity = spotLight[s].getIntensity();
				spotluminosity = spotLight[s].getLuminosity();
				for (int n = 0; n < spotLight[s].getDirectivity(); n++){
					spotintensity *= cospot;
					spotluminosity *= cospot;
				}
				intensity += spotintensity*co*reflectance*dx*dy / r2length;
				luminosity += spotluminosity*co*reflectance*dx*dy / r2length;
			}
			//�[�[�[�m�F�p�[�[�[
			luminosity /= reflectance;
			//�[�[�[�[�[�[�[�[�[
			wallpointLight.setAllParam(center.x, center.y, center.z, color, intensity, 0.034, luminosity);
			cout << "wallpointLight = " <<
				wallpointLight.getPosition() << " , intensity = " << wallpointLight.getIntensity() << " , luminosity = " << wallpointLight.getLuminosity() << endl;
			pointLight.push_back(wallpointLight);
			intensity = 0; luminosity = 0;
			//m��cm�ɖ߂�
			dx = dx*100.0; dy = dy*100.0;
		}
	}
}