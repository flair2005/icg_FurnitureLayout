#define _MYLIGHT_CPP_

#include "myLight.h"
#include "constants.h"

//点光源のシェーディング
Tcolor3<float> PointLight::shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse)
{
	Tvector3<float> vec = position - p;
	//cmをmに変換
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

//平行光源のシェーディング
Tcolor3<float> ParallelLight::shade(Tvector3<float> n, Tcolor3<float> diffuse)
{
	float co = n.dot(direction);
	Tcolor3<float> c = intensity * diffuse * color * co;
	return c;
}

//スポットライトのシェーディング
Tcolor3<float> SpotLight::shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse)
{
	float costhN = 1.0;
	//Tvector3<float> light_axis(0.0, 0.0, -1.0);
	Tvector3<float> vec = position - p;
	//cmをmに変換
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

//一回反射を壁のメッシュの点光源で考える
void calcOnceRefraction(vector<Tvector3<float>>& wallpoint, vector<PointLight>& pointLight, vector<SpotLight>& spotLight,
	int pnum, int snum, int imesh/*横のメッシュの数*/, int jmesh/*縦のメッシュの数*/, float reflectance, Tvector3<float> normal, Tcolor3<float>& color)
{
	Tvector3<float> center, wvec, hvec;
	float dx, dy, width, height;
	PointLight wallpointLight;
	
	wvec = wallpoint[1] - wallpoint[0];
	hvec = wallpoint[3] - wallpoint[0];
	if (normal.x != 0)/*左右の壁*/{
		width = abs(wallpoint[0].y - wallpoint[1].y);
		height = abs(wallpoint[0].z - wallpoint[3].z);
		dx = width / (float)imesh;
		dy = height / (float)jmesh;
		cout << "左右の壁" << endl;
	}
	/*奥行きの壁*/
	else if (normal.y != 0){
		width = abs(wallpoint[0].x - wallpoint[1].x);
		height = abs(wallpoint[0].z - wallpoint[3].z);
		dx = width / (float)imesh;
		dy = height / (float)jmesh;
		cout << "奥，手前の壁" << endl;
	}
	/*天井*/
	else if (normal.z != 0){
		width = abs(wallpoint[0].x - wallpoint[1].x);
		height = abs(wallpoint[0].y - wallpoint[3].y);
		dx = width / (float)imesh;
		dy = height / (float)jmesh;
		cout << "天井" << endl;
	}

	// intensity:shadeのための光源の強度，luminosity:照度のための光度
	float intensity = 0, luminosity = 0, r2length, co, cospot, spotintensity, spotluminosity;
	// 照明と壁光源の距離，後で単位ベクトルにします
	Tvector3<float> r;
	for (int i = 0; i < imesh; i++){
		for (int j = 0; j < jmesh; j++){
			center = wallpoint[0] + ((i + 0.5)*dx / width)*wvec + ((j + 0.5)*dy / height)*hvec;
			//cmをmに変換
			dx = dx / 100.0; dy = dy / 100.0;
			//pointLightのときの壁の反射
			for (int p = 0; p < pnum; p++){
				r = pointLight[p].getPosition() - center; r = r / 100.0;	/*cmをmに変換*/
				r2length = r.lengthSquared(); r = r.normalize();
				if (r2length < 1){
					r2length = 1.0;
				}
				co = r.dot(normal);
				intensity += pointLight[p].getIntensity()*co*reflectance*dx*dy / r2length;
				luminosity += pointLight[p].getLuminosity()*co*reflectance*dx*dy / r2length;
			}
			//spotLightのときの壁の反射
			for (int s = 0; s < snum; s++){
				r = spotLight[s].getPosition() - center; r = r / 100.0;		/*cmをmに変換*/
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
			//ーーー確認用ーーー
			luminosity /= reflectance;
			//ーーーーーーーーー
			wallpointLight.setAllParam(center.x, center.y, center.z, color, intensity, 0.034, luminosity);
			cout << "wallpointLight = " <<
				wallpointLight.getPosition() << " , intensity = " << wallpointLight.getIntensity() << " , luminosity = " << wallpointLight.getLuminosity() << endl;
			pointLight.push_back(wallpointLight);
			intensity = 0; luminosity = 0;
			//mをcmに戻す
			dx = dx*100.0; dy = dy*100.0;
		}
	}
}