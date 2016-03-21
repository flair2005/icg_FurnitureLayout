#ifndef __CALCSHADE_H__
#define __CALCSHADE_H__

#include "myLight.h"

void calcPointLightIntensity(vector<PointLight>& light, Tobject& obj);
//pointLightとspotLightの位置を調節してるやつ
void calcPointLightIntensity2(vector<PointLight>& light, Tobject& obj);
//物体の移動も考慮する
void calcPointLightIntensity3(vector<PointLight>& light, Furniture& objects);

void calcParallelLightIntensity(vector<ParallelLight>& light, Tobject& obj);

void calcSpotLightIntensity(vector<SpotLight>& light, Tobject& obj);

//pointLightとspotLightの位置を調節してるやつ
void calcSpotLightIntensity2(vector<SpotLight>& light, Tobject& obj);

//物体の移動も考慮する
void calcSpotLightIntensity3(vector<SpotLight>& light, Furniture& obj);


void calcAllLightIntensity_floor(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj);
void calcAllLightIntensity_wall(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj);

//pointLightとspotLightの位置を調節してるやつ
void calcAllLightIntensity2(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj);

//物体の移動も考慮する
void calcAllLightIntensity3(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Furniture& objects);


#endif