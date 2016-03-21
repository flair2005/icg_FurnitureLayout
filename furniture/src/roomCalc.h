#ifndef _ROOMCALC_H_
#define _ROOMCALC_H_

#include "icg.h"
#include "Furniture.h"
#include "myLight.h"
#include<opencv2\opencv.hpp>



void saveImage(const unsigned int imageWidth, const unsigned int imageHeight, cv::Mat& gl_outImage);
void mallocFree();
float calcClearance(int window_size_x, int window_size_y);
float calcCirculation(void);
float calcPairRelation(Furniture& objects,float& mpd,float& mpa);
float calcConversation(Furniture& objects,float& mcd,float& mca);
float calcVisualBalance(Furniture& objects);
void calcAlignment(Furniture& objects, float& mfa,float& mwa);
float calcEmphasis(Furniture& objects);
float calcHopeDistribution(Furniture& objects);
float calcHopeDistribution3(Furniture& objects);
float rand_normal(float mu, float sigma);
void authoring(Furniture& objects);
void authoring_befor(Furniture& objects);

float calcLightDistribution(Furniture& objects, vector<PointLight>& pointLight, vector<SpotLight>& spotLight);

//test用
float calcLightDistribution2(Furniture& objects, vector<PointLight>& light);
void testIlluminance(vector<PointLight>& light, Furniture& objects);

//1点だけの照度を考慮した評価関数
float calcIlluminanceDistribution(Furniture& objects, vector<PointLight>& light);

//平均照度を考慮した評価関数
float calcAverageIlluminanceDistribution(vector<vector<float>>& livingFluxTable, vector<vector<float>>& coffeeFluxTable, float living_height, float coffee_height, int livingNum, int coffeeNum, Furniture& objects);

#endif
