#include "myLight.h"
#include "Furniture.h"

void calcPointLightIntensity(vector<PointLight>& light, Tobject& obj)
{
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < light.size(); j++){
			if (j == 0){
				c = light[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += light[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
		}
		vtx.setColor(c);
	}
}

void calcPointLightIntensity2(vector<PointLight>& light, Tobject& obj)
{
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < light.size(); j++){
			if (j == 0){
				c = light[j].shade(vtx.getPosition() + Tvector3<float>(light[j].getPosition().x, light[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += light[j].shade(vtx.getPosition() + Tvector3<float>(light[j].getPosition().x, light[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
			}
		}
		vtx.setColor(c);
	}
}

//ï®ëÃÇÃà⁄ìÆÇ‡çló∂Ç∑ÇÈ
void calcPointLightIntensity3(vector<PointLight>& light, Furniture& objects)
{
	Tcolor3<float> c;
	Tvector3<float> trans_vtx;
	Tvector3<float> base_vtx;
	for (int i = 0; i < objectNum; i++){
		Tmaterial& mat = objects.obj[i].getMaterial();
		Tvector3<float> center = objects.obj[i].getCenter();
		float tx = objects.trans[i].x;
		float ty = objects.trans[i].y;
		float tz = objects.trans[i].z;
		for (int j = 0; j < objects.obj[i].getNumVertex(); j++){
			Tvertex& vtx = objects.obj[i].getVertex(j);
			base_vtx = vtx.getPosition();
			trans_vtx.x = ((base_vtx.x - center.x)*cos(objects.zrot[i]) - (base_vtx.y - center.y)*sin(objects.zrot[i])) + center.x + tx;
			trans_vtx.y = ((base_vtx.x - center.x)*sin(objects.zrot[i]) + (base_vtx.y - center.y)*cos(objects.zrot[i])) + center.y + ty;
			trans_vtx.z = base_vtx.z - center.z + center.z + tz;
			for (int k = 0; k < light.size(); k++){
				if (k == 0)			c  = light[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
				else if (k != 0)	c += light[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
			}
			vtx.setColor(c);
		}
	}
}

void calcParallelLightIntensity(vector<ParallelLight>& light, Tobject& obj)
{
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < light.size(); j++){
			if (j == 0){
				c = light[j].shade(vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += light[j].shade(vtx.getNormal(), mat.getDiffuse());
			}
		}
		vtx.setColor(c);
	}
}

void calcSpotLightIntensity(vector<SpotLight>& light, Tobject& obj)
{
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < light.size(); j++){
			if (j == 0){
				c = light[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += light[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			c += light[j].getAmbient();
		}
		vtx.setColor(c);
	}
}

void calcSpotLightIntensity2(vector<SpotLight>& light, Tobject& obj)
{
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < light.size(); j++){
			if (j == 0){
				c = light[j].shade(vtx.getPosition() + Tvector3<float>(light[j].getPosition().x, light[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += light[j].shade(vtx.getPosition() + Tvector3<float>(light[j].getPosition().x, light[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
			}
			c += light[j].getAmbient();
		}
		vtx.setColor(c);
	}
}

void calcSpotLightIntensity3(vector<SpotLight>& light, Furniture& objects)
{
	Tcolor3<float> c;
	Tvector3<float> trans_vtx;
	Tvector3<float> base_vtx;
	for (int i = 0; i < objectNum; i++){
		Tmaterial& mat = objects.obj[i].getMaterial();
		Tvector3<float> center = objects.obj[i].getCenter();
		float tx = objects.trans[i].x;
		float ty = objects.trans[i].y;
		float tz = objects.trans[i].z;
		for (int j = 0; j < objects.obj[i].getNumVertex(); j++){
			Tvertex& vtx = objects.obj[i].getVertex(j);
			base_vtx = vtx.getPosition();
			trans_vtx.x = ((base_vtx.x - center.x)*cos(objects.zrot[i]) - (base_vtx.y - center.y)*sin(objects.zrot[i])) + center.x + tx;
			trans_vtx.y = ((base_vtx.x - center.x)*sin(objects.zrot[i]) + (base_vtx.y - center.y)*cos(objects.zrot[i])) + center.y + ty;
			trans_vtx.z = base_vtx.z - center.z + center.z + tz;
			for (int k = 0; k < light.size(); k++){
				if (k == 0)			c  = light[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
				else if (k != 0)	c += light[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
			}
			vtx.setColor(c);
		}
	}

}

void calcAllLightIntensity_floor(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj)
{
	//Tcolor3<float> c(0.0,0.0,0.0);
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < pointLight.size(); j++){
			if (j == 0){
				c = pointLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += pointLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			//c += pointLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
		}

		for (int j = 0; j < parallelLight.size(); j++){
			c += parallelLight[j].shade(vtx.getNormal(), mat.getDiffuse());
		}

		for (int j = 0; j < spotLight.size(); j++){
			c += spotLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			c += spotLight[j].getAmbient();
		}
		vtx.setColor(c);
	}
}

void calcAllLightIntensity_wall(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj)
{
	//Tcolor3<float> c(0.0,0.0,0.0);
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < pointLight.size(); j++){
			if (j == 0){
				c = pointLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += pointLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			}
			//c += pointLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
		}

		for (int j = 0; j < parallelLight.size(); j++){
			c += parallelLight[j].shade(vtx.getNormal(), mat.getDiffuse());
		}

		for (int j = 0; j < spotLight.size(); j++){
			c += spotLight[j].shade(vtx.getPosition(), vtx.getNormal(), mat.getDiffuse());
			c += spotLight[j].getAmbient();
		}
		//c /= 3.0;
		vtx.setColor(c);
	}
}
void calcAllLightIntensity2(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj)
{
	Tcolor3<float> c;
	Tmaterial& mat = obj.getMaterial();
	for (int i = 0; i < obj.getNumVertex(); i++){
		Tvertex& vtx = obj.getVertex(i);
		for (int j = 0; j < pointLight.size(); j++){
			if (j == 0){
				c = pointLight[j].shade(vtx.getPosition() + Tvector3<float>(pointLight[j].getPosition().x, pointLight[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
			}
			else if (j != 0){
				c += pointLight[j].shade(vtx.getPosition() + Tvector3<float>(pointLight[j].getPosition().x, pointLight[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
			}
		}

		for (int j = 0; j < parallelLight.size(); j++){
			c += parallelLight[j].shade(vtx.getNormal(), mat.getDiffuse());
		}

		for (int j = 0; j < spotLight.size(); j++){
			c += spotLight[j].shade(vtx.getPosition() + Tvector3<float>(spotLight[j].getPosition().x, spotLight[j].getPosition().y, 0), vtx.getNormal(), mat.getDiffuse());
		}
		vtx.setColor(c);
	}
}

//âÒì]Ç∆Ç©à⁄ìÆÇçló∂ÇµÇΩshade
void calcAllLightIntensity3(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Furniture& objects)
{
	Tcolor3<float> c;
	Tvector3<float> trans_vtx;
	Tvector3<float> base_vtx;
	for (int i = 0; i < objectNum; i++){
		Tmaterial& mat = objects.obj[i].getMaterial();
		Tvector3<float> center = objects.obj[i].getCenter();
		float tx = objects.trans[i].x;
		float ty = objects.trans[i].y;
		float tz = objects.trans[i].z;
		for (int j = 0; j < objects.obj[i].getNumVertex(); j++){
			Tvertex& vtx = objects.obj[i].getVertex(j);
			base_vtx = vtx.getPosition();
			trans_vtx.x = ((base_vtx.x - center.x)*cos(objects.zrot[i]) - (base_vtx.y - center.y)*sin(objects.zrot[i])) + center.x + tx;
			trans_vtx.y = ((base_vtx.x - center.x)*sin(objects.zrot[i]) + (base_vtx.y - center.y)*cos(objects.zrot[i])) + center.y + ty;
			trans_vtx.z = base_vtx.z - center.z + center.z + tz;
			for (int k = 0; k < pointLight.size(); k++){
				if (k == 0)			c  = pointLight[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
				else if (k != 0)	c += pointLight[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
			}

			for (int k = 0; k < spotLight.size(); k++){
				c += spotLight[k].shade(trans_vtx, vtx.getNormal(), mat.getDiffuse());
				c += spotLight[k].getAmbient();
			}

			for (int k = 0; k < parallelLight.size(); k++){
				c += parallelLight[k].shade(vtx.getNormal(), mat.getDiffuse());
			}
			vtx.setColor(c);

		}
	}
}