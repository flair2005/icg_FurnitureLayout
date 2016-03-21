#ifndef __CALCSHADE_H__
#define __CALCSHADE_H__

#include "myLight.h"

void calcPointLightIntensity(vector<PointLight>& light, Tobject& obj);
//pointLight��spotLight�̈ʒu�𒲐߂��Ă���
void calcPointLightIntensity2(vector<PointLight>& light, Tobject& obj);
//���̂̈ړ����l������
void calcPointLightIntensity3(vector<PointLight>& light, Furniture& objects);

void calcParallelLightIntensity(vector<ParallelLight>& light, Tobject& obj);

void calcSpotLightIntensity(vector<SpotLight>& light, Tobject& obj);

//pointLight��spotLight�̈ʒu�𒲐߂��Ă���
void calcSpotLightIntensity2(vector<SpotLight>& light, Tobject& obj);

//���̂̈ړ����l������
void calcSpotLightIntensity3(vector<SpotLight>& light, Furniture& obj);


void calcAllLightIntensity_floor(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj);
void calcAllLightIntensity_wall(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj);

//pointLight��spotLight�̈ʒu�𒲐߂��Ă���
void calcAllLightIntensity2(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Tobject& obj);

//���̂̈ړ����l������
void calcAllLightIntensity3(vector<PointLight>& pointLight, vector<ParallelLight>& parallelLight, vector<SpotLight>& spotLight, Furniture& objects);


#endif